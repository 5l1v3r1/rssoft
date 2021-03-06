/*
 Copyright 2013 Edouard Griffiths <f4exb at free dot fr>

 This file is part of RSSoft. A Reed-Solomon Soft Decoding library

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA

 Utility routines not linked to a patricular GF entity

 */

#include "GF_Utils.h"

namespace rssoft
{
namespace gf
{

// ================================================================================================
bool binomial_coeff_parity(unsigned int n, unsigned int k)
{
	while (k>0)
	{
		if ((n%2 == 0) && (k%2 == 1))
		{
			return true;
		}

		n /= 2;
		k /= 2;
	}

	return false; // k = 0
}

// ================================================================================================
unsigned int factorial(unsigned int x, unsigned int result) 
{
    if (x == 0)
    {
        return 1;
    }
    else if (x == 1)
    {
        return result; 
    }
    else 
    {
        return factorial(x - 1, x * result);
    }
}

// ================================================================================================
unsigned int binomial_coeff(unsigned int n, unsigned int k)
{
    if (n<k)
    {
        return 0;
    }
    else
    {
        return factorial(n) / (factorial(k) * factorial(n-k));
    }
}

// ================================================================================================
void print_symbols_vector(std::ostream& os, const std::vector<GFq_Symbol>& v)
{
	std::vector<rssoft::gf::GFq_Symbol>::const_iterator c_it = v.begin();
    os << "[";

    for (; c_it != v.end(); ++c_it)
    {
        if (c_it != v.begin())
        {
            os << ", ";
        }

        os <<  *c_it;
    }

    os << "]";
}

// ================================================================================================
void print_elements_vector(std::ostream& os, const std::vector<GFq_Element>& v)
{
	std::vector<rssoft::gf::GFq_Element>::const_iterator c_it = v.begin();
    os << "[";

    for (; c_it != v.end(); ++c_it)
    {
        if (c_it != v.begin())
        {
            os << ", ";
        }

        os <<  *c_it;
    }

    os << "]";
}

// ================================================================================================
bool compare_symbol_vectors(const std::vector<GFq_Symbol>& v1, const std::vector<GFq_Symbol>& v2)
{
	if (v1.size() != v2.size())
	{
		return false;
	}

	for (unsigned int i=0; i<v1.size(); i++)
	{
		if (v1[i] != v2[i])
		{
			return false;
		}
	}

	return true;
}

// ================================================================================================
void print_symbols_and_erasures(std::ostream& os, const std::vector<GFq_Symbol>& v, std::set<unsigned int>& erasure_indexes)
{
	std::vector<rssoft::gf::GFq_Symbol>::const_iterator c_it = v.begin();
    unsigned int i_c = 0;
    os << "[";

    for (; c_it != v.end(); ++c_it, i_c++)
    {
        if (c_it != v.begin())
        {
            os << ", ";
        }

        if (erasure_indexes.find(i_c) == erasure_indexes.end())
        {
            os <<  *c_it;
        }
        else
        {
            os << "*";
        }
    }

    os << "]";
}

} // namespace gf
} // namespace rssoft
