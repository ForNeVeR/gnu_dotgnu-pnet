/*
 * SurrogateSelector.cs - Implementation of the
 *			"System.Runtime.Serialization.SurrogateSelector" interface.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

#if !ECMA_COMPAT

public class SurrogateSelector : ISurrogateSelector
{
	// Internal state.
	private ISurrogateSelector nextSelector;

	// Constructor.
	[TODO]
	public SurrogateSelector()
			{
				// TODO
			}

	// Implement the ISurrogateSelector interface.
	[TODO]
	public virtual void ChainSelector(ISurrogateSelector selector)
			{
				// TODO
			}
	public virtual ISurrogateSelector GetNextSelector()
			{
				return nextSelector;
			}
	public virtual ISerializationSurrogate GetSurrogate
		(Type type, StreamingContext context, out ISurrogateSelector selector)
			{
				// TODO
				return null;
			}

	// Add a surrogate for a specific type.
	public virtual void RemoveSurrogate(Type type, StreamingContext context,
									    ISerializationSurrogate surrogate)
			{
				// TODO
			}

	// Remove the surrogate for a specific type.
	public virtual void RemoveSurrogate(Type type, StreamingContext context)
			{
				// TODO
			}

}; // class SurrogateSelector

#endif // !ECMA_COMPAT

}; // namespace System.Runtime.Serialization
