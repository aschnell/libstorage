/*
 * Copyright (c) 2010 Novell, Inc.
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef ENUM_H
#define ENUM_H


#include <assert.h>
#include <string>
#include <vector>
#include <algorithm>

#include "storage/StorageInterface.h"


namespace storage
{
    using std::string;
    using std::vector;


    template <typename EnumType> struct EnumInfo {};

    template <> struct EnumInfo<FsType> { static const vector<string> names; };
    template <> struct EnumInfo<PartitionType> { static const vector<string> names; };
    template <> struct EnumInfo<MountByType> { static const vector<string> names; };
    template <> struct EnumInfo<EncryptType> { static const vector<string> names; };
    template <> struct EnumInfo<MdType> { static const vector<string> names; };
    template <> struct EnumInfo<MdParity> { static const vector<string> names; };
    template <> struct EnumInfo<MdArrayState> { static const vector<string> names; };
    template <> struct EnumInfo<UsedByType> { static const vector<string> names; };
    template <> struct EnumInfo<CType> { static const vector<string> names; };
    template <> struct EnumInfo<Transport> { static const vector<string> names; }; 
    template <> struct EnumInfo<ImsmDriver> { static const vector<string> names; }; 
    template <> struct EnumInfo<PartAlign> { static const vector<string> names; };


    template <typename EnumType>
    const string& toString(EnumType value)
    {
	static_assert(std::is_enum<EnumType>::value, "not enum");

	const vector<string>& names = EnumInfo<EnumType>::names;

	// Comparisons must not be done with type of enum since the enum may
	// define comparison operators.
	assert((size_t)(value) < names.size());

	return names[value];
    }


    template <typename EnumType>
    EnumType toValue(const string& str, EnumType fallback)
    {
	static_assert(std::is_enum<EnumType>::value, "not enum");

	const vector<string>& names = EnumInfo<EnumType>::names;

	vector<string>::const_iterator it = find(names.begin(), names.end(), str);

	if (it == names.end())
	    return fallback;

	return EnumType(it - names.begin());
    }

}


#endif
