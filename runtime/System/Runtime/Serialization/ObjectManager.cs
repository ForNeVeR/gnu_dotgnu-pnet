/*
 * ObjectManager.cs - Implementation of the
 *			"System.Runtime.Serialization.ObjectManager" class.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace System.Runtime.Serialization
{

#if CONFIG_SERIALIZATION

using System.Collections;
using System.Reflection;
using System.Security.Permissions;

public class ObjectManager
{
	// Information that is stored for a member fixup.
	private sealed class ObjectFixup
	{
		public ObjectInfo obj;
		public MemberInfo member;
		public String memberName;
		public int[] arrayIndex;
		public ObjectFixup nextFixup;

	}; // class ObjectFixup

	// Information that is stored for an object identifier.
	private sealed class ObjectInfo
	{
		public Object obj;
		public SerializationInfo sinfo;
		public long idOfContainingObject;
		public MemberInfo member;
		public int[] arrayIndex;
		public ObjectInfo contains;
		public ObjectInfo nextContains;
		public ObjectFixup fixups;
		public bool done;

	}; // class ObjectInfo

	// Internal state.
	private ISurrogateSelector selector;
	private StreamingContext context;
	private Hashtable objects;
	private ArrayList callbackList;

	// Constructor.
	public ObjectManager(ISurrogateSelector selector,
						 StreamingContext context)
			{
				// Make sure that we have the correct permissions.
				SecurityPermission perm = new SecurityPermission
					(SecurityPermissionFlag.SerializationFormatter);
				perm.Demand();

				// Initialize the object manager.
				this.selector = selector;
				this.context = context;
				this.objects = new Hashtable();
				this.callbackList = new ArrayList();
			}

	// Apply a fixup.
	private static void ApplyFixup(ObjectInfo oinfo, ObjectFixup fixup)
			{
				if(fixup.obj.obj == null)
				{
					throw new SerializationException
						(_("Serialize_MissingFixup"));
				}
				if(fixup.member != null)
				{
					if(fixup.member is FieldInfo)
					{
						((FieldInfo)(fixup.member)).SetValue
							(oinfo.obj, fixup.obj.obj);
					}
					else
					{
						throw new SerializationException
							(_("Serialize_BadFixup"));
					}
				}
				else if(fixup.memberName != null)
				{
					MemberInfo[] member = oinfo.obj.GetType().GetMember
						(fixup.memberName);
					if(member == null || member.Length != 1)
					{
						throw new SerializationException
							(_("Serialize_BadFixup"));
					}
					if(member[0] is FieldInfo)
					{
						((FieldInfo)(member[0])).SetValue
							(oinfo.obj, fixup.obj.obj);
					}
					else
					{
						throw new SerializationException
							(_("Serialize_BadFixup"));
					}
				}
				else if(fixup.arrayIndex != null)
				{
					((Array)(oinfo.obj)).SetValue
						(fixup.obj.obj, fixup.arrayIndex);
				}
				else
				{
					throw new SerializationException
						(_("Serialize_BadFixup"));
				}
			}

	// Apply a contained member fixup.
	private static void ApplyContained(ObjectInfo oinfo, ObjectInfo contain)
			{
				if(contain.member != null)
				{
					if(contain.member is FieldInfo)
					{
						((FieldInfo)(contain.member)).SetValue
							(oinfo.obj, contain.obj);
					}
					else
					{
						throw new SerializationException
							(_("Serialize_BadFixup"));
					}
				}
				else if(contain.arrayIndex != null)
				{
					((Array)(oinfo.obj)).SetValue
						(contain.obj, contain.arrayIndex);
				}
				else
				{
					throw new SerializationException
						(_("Serialize_BadFixup"));
				}
			}

	// Perform recorded fixups for contained objects.
	private static void DoFixupsForContained(ObjectInfo oinfo)
			{
				ObjectInfo contain = oinfo.contains;
				ObjectFixup fixup;
				do
				{
					if(!(contain.done))
					{
						contain.done = true;
						if(contain.obj == null)
						{
							throw new SerializationException
								(_("Serialize_MissingFixup"));
						}
						if(contain.contains != null)
						{
							DoFixupsForContained(contain);
						}
						fixup = contain.fixups;
						while(fixup != null)
						{
							ApplyFixup(contain, fixup);
							fixup = fixup.nextFixup;
						}
						ApplyContained(oinfo, contain);
					}
					contain = contain.nextContains;
				}
				while(contain != null);
			}

	// Perform recorded fixups.
	public virtual void DoFixups()
			{
				IDictionaryEnumerator e = objects.GetEnumerator();
				ObjectInfo oinfo;
				ObjectFixup fixup;
				while(e.MoveNext())
				{
					oinfo = (ObjectInfo)(e.Value);
					if(oinfo.obj == null)
					{
						throw new SerializationException
							(_("Serialize_MissingFixup"));
					}
					if(oinfo.done || oinfo.idOfContainingObject > 0)
					{
						// We already saw this object or the object is
						// contained within something at a higher level.
						continue;
					}
					oinfo.done = true;
					if(oinfo.contains != null)
					{
						// Handle value type members within this object.
						DoFixupsForContained(oinfo);
					}
					fixup = oinfo.fixups;
					while(fixup != null)
					{
						ApplyFixup(oinfo, fixup);
						fixup = fixup.nextFixup;
					}
				}
			}

	// Return an object with a specific identifier.
	public virtual Object GetObject(long objectID)
			{
				if(objectID <= 0)
				{
					throw new ArgumentOutOfRangeException
						("objectID", _("Serialize_BadObjectID"));
				}
				ObjectInfo info = (ObjectInfo)(objects[objectID]);
				if(info != null)
				{
					return info.obj;
				}
				else
				{
					return null;
				}
			}

	// Raise a deserialization event on all register objects that want it.
	public virtual void RaiseDeserializationEvent()
			{
				IEnumerator e = callbackList.GetEnumerator();
				while(e.MoveNext())
				{
					IDeserializationCallback cb;
					cb = (e.Current as IDeserializationCallback);
					if(cb != null)
					{
						cb.OnDeserialization(null);
					}
				}
			}

	// Record a fixup.
	private void RecordFixup(long objectToBeFixed, long objectRequired,
							 MemberInfo member, String memberName,
							 int[] arrayIndex)
			{
				// Find or create the containing object's information block.
				ObjectInfo oinfo = (ObjectInfo)(objects[objectToBeFixed]);
				if(oinfo == null)
				{
					oinfo = new ObjectInfo();
					objects[objectToBeFixed] = oinfo;
				}

				// Find or create the required object's information block.
				ObjectInfo oinfo2 = (ObjectInfo)(objects[objectRequired]);
				if(oinfo2 == null)
				{
					oinfo2 = new ObjectInfo();
					objects[objectRequired] = oinfo2;
				}

				// Add a fixup to the containing object.
				ObjectFixup fixup = new ObjectFixup();
				fixup.obj = oinfo2;
				fixup.member = member;
				fixup.memberName = memberName;
				fixup.arrayIndex = arrayIndex;
				fixup.nextFixup = oinfo.fixups;
				oinfo.fixups = fixup;
			}

	// Record an array element fixup to be performed later.
	public virtual void RecordArrayElementFixup
				(long arrayToBeFixed, int index, long objectRequired)
			{
				if(arrayToBeFixed <= 0)
				{
					throw new ArgumentOutOfRangeException
						("arrayToBeFixed", _("Serialize_BadObjectID"));
				}
				if(objectRequired <= 0)
				{
					throw new ArgumentOutOfRangeException
						("objectRequired", _("Serialize_BadObjectID"));
				}
				RecordFixup(arrayToBeFixed, objectRequired,
							null, null, new int[] {index});
			}
	public virtual void RecordArrayElementFixup
				(long arrayToBeFixed, int[] indices, long objectRequired)
			{
				if(arrayToBeFixed <= 0)
				{
					throw new ArgumentOutOfRangeException
						("arrayToBeFixed", _("Serialize_BadObjectID"));
				}
				if(objectRequired <= 0)
				{
					throw new ArgumentOutOfRangeException
						("objectRequired", _("Serialize_BadObjectID"));
				}
				if(indices == null)
				{
					throw new ArgumentNullException("indices");
				}
				RecordFixup(arrayToBeFixed, objectRequired,
							null, null, indices);
			}

	// Record an object member fixup to be performed later.
	public virtual void RecordDelayedFixup
				(long objectToBeFixed, String memberName, long objectRequired)
			{
				if(objectToBeFixed <= 0)
				{
					throw new ArgumentOutOfRangeException
						("objectToBeFixed", _("Serialize_BadObjectID"));
				}
				if(objectRequired <= 0)
				{
					throw new ArgumentOutOfRangeException
						("objectRequired", _("Serialize_BadObjectID"));
				}
				if(memberName == null)
				{
					throw new ArgumentNullException("memberName");
				}
				RecordFixup(objectToBeFixed, objectRequired,
							null, memberName, null);
			}
	public virtual void RecordFixup
				(long objectToBeFixed, MemberInfo member, long objectRequired)
			{
				if(objectToBeFixed <= 0)
				{
					throw new ArgumentOutOfRangeException
						("objectToBeFixed", _("Serialize_BadObjectID"));
				}
				if(objectRequired <= 0)
				{
					throw new ArgumentOutOfRangeException
						("objectRequired", _("Serialize_BadObjectID"));
				}
				if(member == null)
				{
					throw new ArgumentNullException("member");
				}
				RecordFixup(objectToBeFixed, objectRequired,
							member, null, null);
			}

	// Register an object with the object manager.
	public virtual void RegisterObject(Object obj, long objectID)
			{
				RegisterObject(obj, objectID, null, 0, null, null);
			}
	public void RegisterObject(Object obj, long objectID,
							   SerializationInfo info)
			{
				RegisterObject(obj, objectID, info, 0, null, null);
			}
	public void RegisterObject(Object obj, long objectID,
							   SerializationInfo info,
							   long idOfContainingObj,
							   MemberInfo member)
			{
				RegisterObject(obj, objectID, info, idOfContainingObj,
							   member, null);
			}
	public void RegisterObject(Object obj, long objectID,
							   SerializationInfo info,
							   long idOfContainingObj,
							   MemberInfo member,
							   int[] arrayIndex)
			{
				if(obj == null)
				{
					throw new ArgumentNullException("objectID");
				}
				if(objectID <= 0)
				{
					throw new ArgumentOutOfRangeException
						("objectID", _("Serialize_BadObjectID"));
				}
				ObjectInfo oinfo = (ObjectInfo)(objects[objectID]);
				if(oinfo != null && oinfo.obj != obj)
				{
					throw new SerializationException
						(_("Serialize_AlreadyRegistered"));
				}
				else if(oinfo != null)
				{
					// Update the information for an existing reference.
					oinfo.obj = obj;
					if(info != null)
					{
						oinfo.sinfo = info;
					}
					if(member != null)
					{
						oinfo.member = member;
					}
					if(arrayIndex != null)
					{
						oinfo.arrayIndex = arrayIndex;
					}
					if(idOfContainingObj != 0 &&
					   oinfo.idOfContainingObject == 0)
					{
						oinfo.idOfContainingObject = idOfContainingObj;
						RegisterWithContaining(oinfo);
					}
				}
				else
				{
					// Create a new object information block.
					oinfo = new ObjectInfo();
					oinfo.obj = obj;
					oinfo.sinfo = info;
					oinfo.idOfContainingObject = idOfContainingObj;
					oinfo.member = member;
					oinfo.arrayIndex = arrayIndex;
					objects[objectID] = oinfo;

					// Register the object to be called later by
					// "RaiseDeserializationEvent".
					if(obj is IDeserializationCallback)
					{
						callbackList.Add(obj);
					}

					// Register the information block with the container.
					if(idOfContainingObj > 0)
					{
						RegisterWithContaining(oinfo);
					}
				}
			}

	// Register an object with its containing object.
	private void RegisterWithContaining(ObjectInfo oinfo)
			{
				ObjectInfo oinfo2 =
					(ObjectInfo)(objects[oinfo.idOfContainingObject]);
				if(oinfo2 == null)
				{
					oinfo2 = new ObjectInfo();
					objects[oinfo.idOfContainingObject] = oinfo2;
				}
				oinfo.nextContains = oinfo2.contains;
				oinfo2.contains = oinfo;
			}

}; // class ObjectManager

#endif // CONFIG_SERIALIZATION

}; // namespace System.Runtime.Serialization
