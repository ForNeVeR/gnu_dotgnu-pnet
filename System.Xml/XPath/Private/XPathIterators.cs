/*
 * XPathIterators.cs - implementation subclasses of XPathNodeIterator.
 *
 * Copyright (C) 2004  Southern Storm Software, Pty Ltd.
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

using System;
using System.Xml;
using System.Xml.XPath;
using System.Collections;

#if CONFIG_XPATH

namespace System.Xml.XPath.Private
{
	internal abstract class XPathBaseIterator : XPathNodeIterator
	{
		private int count = -1;
		public XPathBaseIterator (XPathBaseIterator parent)
		{
		}
		
		public XPathBaseIterator (XmlNamespaceManager nsmanager)
		{
		}

		public override int Count
		{
			get
			{
				lock(this)
				{
					if(count == -1)
					{
						count = 0;
						while(MoveNext())
						{
							count++;
						}
					}
				}
				return count;
			}
		}

	}

	internal abstract class XPathSimpleIterator : XPathBaseIterator
	{
		protected readonly XPathNavigator navigator;
		protected readonly XPathBaseIterator parent;
		protected XPathNavigator current;
		protected int pos;

		public XPathSimpleIterator(XPathBaseIterator parent) : base (parent)
		{
			this.parent = parent;
			navigator = parent.Current.Clone();
			current = navigator.Clone();
			pos = 0;
		}

		public XPathSimpleIterator(XPathNavigator navigator, 
								   XmlNamespaceManager nsmanager) 
								   : base(nsmanager)
		{
			this.navigator = navigator.Clone();
			current = navigator.Clone();
		}
		
		public override XPathNavigator Current
		{
			get
			{
				return current;
			}
		}

		public override int CurrentPosition
		{
			get
			{
				return pos;
			}
		}
	}

	internal class XPathSelfIterator : XPathSimpleIterator
	{
		public XPathSelfIterator(XPathBaseIterator parent) : base(parent)
		{
		}

		public XPathSelfIterator(XPathNavigator navigator, 
								 XmlNamespaceManager nsmanager) 
								 : base(navigator, nsmanager)
		{
		}

		public XPathSelfIterator (XPathSelfIterator copy) : base(copy) 
		{
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathSelfIterator(this);
		}

		public override bool MoveNext()
		{
			if(pos == 0)
			{
				pos = 1;
				return true;
			}
			return false;
		}
	}

	internal class XPathChildIterator : XPathSimpleIterator
	{
		public XPathChildIterator (XPathBaseIterator iterator) : base (iterator)
		{
		}

		public XPathChildIterator(XPathChildIterator copy) : base (copy)
		{
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathChildIterator(this);
		}

		public override bool MoveNext()
		{
			if(pos == 0)
			{
				if(navigator.MoveToFirstChild())
				{
					pos++;
					current = navigator.Clone();
					return true;
				}
			}
			else if(navigator.MoveToNext())
			{
				pos++;
				current = navigator.Clone();
				return true;
			}

			return false;
		}
	}
	
	internal class XPathAttributeIterator : XPathSimpleIterator
	{
		public XPathAttributeIterator (XPathBaseIterator iterator) : base (iterator)
		{
		}

		public XPathAttributeIterator(XPathAttributeIterator copy) : base (copy)
		{
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathAttributeIterator(this);
		}

		public override bool MoveNext()
		{
			if(pos == 0)
			{
				if(navigator.MoveToFirstAttribute())
				{
					pos++;
					current = navigator.Clone();
					return true;
				}
			}
			else if(navigator.MoveToNextAttribute())
			{
				pos++;
				current = navigator.Clone();
				return true;
			}

			return false;
		}
	}
	
	internal class XPathAncestorIterator : XPathSimpleIterator
	{
		ArrayList parents = null;

		public XPathAncestorIterator (XPathBaseIterator iterator) : base (iterator)
		{
		}

		public XPathAncestorIterator(XPathAncestorIterator copy) : base (copy)
		{
			// TODO : do we need to clone this ?
			if(this.parents != null)
			{
				this.parents = (ArrayList)this.parents.Clone();
			}
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathAncestorIterator(this);
		}

		public override bool MoveNext()
		{
			if(parents == null)
			{
				parents = new ArrayList();
				while(navigator.MoveToParent())
				{
					if(navigator.NodeType == XPathNodeType.Root)
					{
						break;
					}
					// TODO: duplicate check
					parents.Add(navigator.Clone());
				}
				parents.Reverse();
			}

			if(pos < parents.Count)
			{
				current = (XPathNavigator)parents[pos];
				pos++;
				return true;
			}

			return false;
		}
	}
	
	internal class XPathAncestorOrSelfIterator : XPathSimpleIterator
	{
		ArrayList parents = null;

		public XPathAncestorOrSelfIterator (XPathBaseIterator iterator) : base (iterator)
		{
		}

		public XPathAncestorOrSelfIterator(XPathAncestorOrSelfIterator copy) : base (copy)
		{
			// TODO : do we need to clone this ?
			if(this.parents != null)
			{
				this.parents = (ArrayList)this.parents.Clone();
			}
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathAncestorOrSelfIterator(this);
		}

		public override bool MoveNext()
		{
			if(parents == null)
			{
				parents = new ArrayList();
				parents.Add(navigator.Clone());
				while(navigator.MoveToParent())
				{
					if(navigator.NodeType == XPathNodeType.Root)
					{
						break;
					}
					// TODO: duplicate check
					parents.Add(navigator.Clone());
				}
				parents.Reverse();
			}

			if(pos < parents.Count)
			{
				current = (XPathNavigator)parents[pos];
				pos++;
				return true;
			}

			return false;
		}
	}

	internal class XPathParentIterator : XPathSimpleIterator
	{
		public XPathParentIterator (XPathBaseIterator iterator) : base (iterator)
		{
		}

		public XPathParentIterator(XPathParentIterator copy) : base (copy)
		{
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathParentIterator(this);
		}

		public override bool MoveNext()
		{
			if(pos == 0)
			{
				if(navigator.MoveToParent())
				{
					pos++;
					current = navigator.Clone();
					return true;
				}
			}

			return false;
		}
	}

	internal class XPathDescendantIterator : XPathSimpleIterator
	{
		private int depth  = 0 ;
		private bool finished = false; 
		
		public XPathDescendantIterator (XPathBaseIterator iterator) : base (iterator)
		{
		}

		public XPathDescendantIterator(XPathDescendantIterator copy) : base (copy)
		{
			this.depth = copy.depth;
			this.finished = copy.finished;
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathDescendantIterator(this);
		}

		public override bool MoveNext()
		{
			// This is needed as at the end of the loop, the original will be restored 
			// as the navigator value.
			if(finished) 
			{
				return false;
			}

			if(navigator.MoveToFirstChild())
			{
				depth++;
				pos++;
				current = navigator.Clone();
				return true;
			}
			while(depth != 0)
			{
				if(navigator.MoveToNext())
				{
					pos++;
					current = navigator.Clone();
					return true;
				}
				if(!navigator.MoveToParent())
				{
					// TODO: resources
					throw new XPathException("there should be parent for depth != 0" , null);
				}
				depth--;
			}

			finished = true;

			return false;
		}
	}

	internal class XPathDescendantOrSelfIterator : XPathSimpleIterator
	{
		private int depth  = 0 ;
		private bool finished = false; 
		
		public XPathDescendantOrSelfIterator (XPathBaseIterator iterator) : base (iterator)
		{
		}

		public XPathDescendantOrSelfIterator(XPathDescendantOrSelfIterator copy) : base (copy)
		{
			this.depth = copy.depth;
			this.finished = copy.finished;
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathDescendantOrSelfIterator(this);
		}

		public override bool MoveNext()
		{
			// This is needed as at the end of the loop, the original 
			// will be restored as the navigator value.
			if(finished) 
			{
				return false;
			}

			if(pos == 0)
			{
				pos++;
				current = navigator.Clone();
				return true;
			}

			if(navigator.MoveToFirstChild())
			{
				depth++;
				pos++;
				current = navigator.Clone();
				return true;
			}
			while(depth != 0)
			{
				if(navigator.MoveToNext())
				{
					pos++;
					current = navigator.Clone();
					return true;
				}
				if(!navigator.MoveToParent())
				{
					// TODO: resources
					throw new XPathException("there should be parent for depth != 0" , null);
				}
				depth--;
			}

			finished = true;

			return false;
		}
	}


	internal class XPathAxisIterator : XPathBaseIterator
	{
		protected XPathSimpleIterator iterator;
		protected NodeTest test;
		protected int pos;

		public XPathAxisIterator(XPathSimpleIterator iterator, NodeTest test)
			: base(iterator)
		{
			this.iterator = iterator;
			this.test = test;
		}

		public XPathAxisIterator(XPathAxisIterator copy)
			: base(copy)
		{
			iterator = (XPathSimpleIterator) (copy.iterator.Clone());
			test = copy.test;
			pos = copy.pos;
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathAxisIterator(this);
		}

		public override bool MoveNext()
		{
			String name = null; 
			String ns = null;
			
			if(test.name != null)
			{
				name = test.name.Name;
				ns = test.name.Namespace;
			}
			while(iterator.MoveNext())
			{
				if(test.nodeType != XPathNodeType.All && 
						test.nodeType != Current.NodeType)
				{
					continue;
				}
				// TODO: namespace lookups using namespace manager
				if(ns != null && Current.NamespaceURI != ns)
				{
					continue;
				}
				if(name != null && Current.LocalName != name)
				{
					continue;
				}
				pos++;
				return true;
			}
			return false;
		}

		public override XPathNavigator Current 
		{
			get
			{
				return iterator.Current;
			}
		}

		public override int CurrentPosition
		{
			get
			{
				return pos;
			}
			
		}
	}

	internal class XPathSlashIterator : XPathBaseIterator 
	{
		protected XPathBaseIterator lhs;
		protected XPathBaseIterator rhs = null;
		protected Expression  expr;
		protected int pos;

		public XPathSlashIterator (XPathBaseIterator lhs, Expression expr)
			: base(lhs)
		{
			this.lhs = 	lhs;
			this.expr =  expr;
		}

		public XPathSlashIterator(XPathSlashIterator copy)
			: base(copy)
		{
			this.lhs = (XPathBaseIterator) copy.lhs.Clone();
			this.expr = copy.expr;
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathSlashIterator(this);
		}

		public override bool MoveNext()
		{
			//TODO: depth first traversal - fix when doing Evaluate
			while(rhs == null || !rhs.MoveNext())
			{
				if(!lhs.MoveNext())
				{
					return false;
				}
				rhs = (XPathBaseIterator)expr.Evaluate(lhs);
			}
			
			pos++;
			// We have already done an rhs.MoveNext()
			return true;
		}

		public override XPathNavigator Current
		{
			get
			{
				return rhs.Current;
			}
		}

		public override int CurrentPosition
		{
			get
			{
				return pos;
			}
		}
	}

	internal class XPathPredicateIterator : XPathBaseIterator
	{
		protected XPathBaseIterator iterator;
		protected Expression predicate;
		protected int pos;
		protected XPathResultType resultType;

		public XPathPredicateIterator(XPathBaseIterator iterator,
									  Expression predicate) 
									  : base (iterator)
		{
			this.iterator = iterator;
			this.predicate = predicate;
			this.resultType = predicate.ReturnType;
		}

		public XPathPredicateIterator(XPathPredicateIterator copy) : base(copy)
		{
			this.iterator = (XPathBaseIterator) copy.iterator.Clone();
			this.predicate = copy.predicate;
			this.resultType = copy.resultType;
			this.pos = copy.pos;
		}

		public override XPathNodeIterator Clone()
		{
			return new XPathPredicateIterator(this);
		}

		public override bool MoveNext()
		{
			while(iterator.MoveNext())
			{
				bool match = false;
				switch(predicate.ReturnType)
				{
					case XPathResultType.String:
					case XPathResultType.NodeSet:
					case XPathResultType.Boolean:
					{
						match = (bool)predicate.EvaluateAs(iterator, XPathResultType.Boolean);
					}
					break;
					case XPathResultType.Number:
					{
						match = (iterator.CurrentPosition)  == (double)predicate.Evaluate(iterator);
					}
					break;
					default:
					{
						throw new NotSupportedException("TODO: " + predicate.ReturnType);
					}
					break;
				}
				if(match)
				{
					pos++;
					return true;
				}
			}
			return false;
		}

		public override XPathNavigator Current
		{
			get
			{
				return iterator.Current;
			}
		}

		public override int CurrentPosition
		{
			get
			{
				return pos;
			}
		}
	}					

}

#endif /* CONFIG_XPATH */
