/*
 * HttpStatusCode.cs - Implementation of the "System.Net.HttpStatusCode" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Net
{

public enum HttpStatusCode
{
	Accepted = 202,
	Ambiguous = 300,
	BadGateway = 502,
	BadRequest = 400,
	Conflict = 409,
	Continue = 100,
	Created = 201,
	ExpectationFailed = 417,
	Forbidden = 403,
	Found = 302,
	GatewayTimeout = 504,
	Gone = 410,
	HttpVersionNotSupported = 505,
	InternalServerError = 500,
	LengthRequired = 411,
	MethodNotAllowed = 405,
	Moved = 301,
	MovedPermanently = 301,
	MultipleChoices = 300,
	NoContent = 204,
	NonAuthoritativeInformation = 203,
	NotAcceptable = 406,
	NotFound = 404,
	NotImplemented = 501,
	NotModified = 304,
	OK = 200,
	PartialContent = 206,
	PaymentRequired = 402,
	PreconditionFailed = 412,
	ProxyAuthenticationRequired = 407,
	Redirect = 302,
	RedirectKeepVerb = 307,
	RedirectMethod = 303,
	RequestEntityTooLarge = 413,
	RequestTimeout = 408,
	RequestUriTooLong = 414,
	RequestedRangeNotSatisfiable = 416,
	ResetContent = 205,
	SeeOther = 303,
	ServiceUnavailable = 503,
	SwitchingProtocols = 101,
	TemporaryRedirect = 307,
	Unauthorized = 401,
	UnsupportedMediaType = 415,
	Unused = 306,
	UseProxy = 305
}; // enum HttpStatusCode

}; // namespace System.Net
