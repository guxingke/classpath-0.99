/* UnionMemberHelper.java --
   Copyright (C) 2005 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package org.omg.CORBA;

import gnu.CORBA.Minor;
import gnu.CORBA.OrbRestricted;
import gnu.CORBA.TypeCodeHelper;

import org.omg.CORBA.portable.InputStream;
import org.omg.CORBA.portable.OutputStream;

/**
 * A helper operations for the union member.
 *
 * @author Audrius Meskauskas, Lithuania (AudriusA@Bioinformatics.org)
 */
public abstract class UnionMemberHelper
{
  /**
   * The cached typecode value (computed once).
   */
  private static TypeCode typeCode;

  /**
   * Used in creating recursive unions.
   */
  private static boolean active;

  /**
   * Inset the union member into the given Any.
   */
  public static void insert(Any a, UnionMember that)
  {
    OutputStream out = a.create_output_stream();
    a.type(type());
    write(out, that);
    a.read_value(out.create_input_stream(), type());
  }

  /**
   * Extract the union member from the given Any.
   */
  public static UnionMember extract(Any a)
  {
    return read(a.create_input_stream());
  }

  /**
   * Create and return the typecode for this union member.
   */
  public static synchronized TypeCode type()
  {
    if (typeCode == null)
      {
        synchronized (TypeCode.class)
          {
            if (typeCode == null)
              {
                if (active)
                  {
                    return OrbRestricted.Singleton.create_recursive_tc(id());
                  }
                active = true;

                ORB orb = OrbRestricted.Singleton;

                StructMember[] members = new StructMember[ 4 ];
                TypeCode member;
                member = orb.create_string_tc(0);
                member =
                  orb.create_alias_tc(IdentifierHelper.id(), "Identifier",
                                      member
                                     );
                members [ 0 ] = new StructMember("name", member, null);
                member = orb.get_primitive_tc(TCKind.tk_any);
                members [ 1 ] = new StructMember("label", member, null);
                member = orb.create_string_tc(0);
                member = orb.get_primitive_tc(TCKind.tk_TypeCode);
                members [ 2 ] = new StructMember("type", member, null);
                member = IDLTypeHelper.type();
                members [ 3 ] = new StructMember("type_def", member, null);
                typeCode =
                  orb.create_struct_tc(UnionMemberHelper.id(), "UnionMember",
                                       members
                                      );
                active = false;
              }
          }
      }
    return typeCode;
  }

  /**
   * Return the UnionMember repository id.
   *
   * @return "IDL:omg.org/CORBA/UnionMember:1.0", always.
   */
  public static String id()
  {
    return "IDL:omg.org/CORBA/UnionMember:1.0";
  }

  /**
   * Read the union member from the given stream.
   */
  public static UnionMember read(InputStream istream)
  {
    try
      {
        UnionMember value = new UnionMember();
        value.name = istream.read_string();
        value.label = istream.read_any();
        value.type = TypeCodeHelper.read(istream);
        value.type_def = IDLTypeHelper.read(istream);
        return value;
      }
    catch (UserException ex)
      {
        MARSHAL m = new MARSHAL();
        m.minor = Minor.UserException;
        m.initCause(ex);
        throw m;
      }
  }

  /**
   * Write the union member to the given stream.
   */
  public static void write(OutputStream ostream, UnionMember value)
  {
    try
      {
        ostream.write_string(value.name);
        ostream.write_any(value.label);
        TypeCodeHelper.write(ostream, value.type);
        IDLTypeHelper.write(ostream, value.type_def);
      }
    catch (UserException ex)
      {
        MARSHAL m = new MARSHAL();
        m.minor = Minor.UserException;
        m.initCause(ex);
        throw m;
      }
  }
}
