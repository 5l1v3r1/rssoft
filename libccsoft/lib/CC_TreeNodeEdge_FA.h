/*
 Copyright 2013 Edouard Griffiths <f4exb at free dot fr>

 This file is part of CCSoft. A Convolutional Codes Soft Decoding library

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

 Combination of a Node and its incoming Edge in the convolutional code tree
 (node+edge combo). In a tree structure you don't need to store nodes and
 edges as nodes have a single incoming edge. So a node can incorporate
 its incoming edge.

 This version uses fixed arrays for registers and forward node-edges

 */
#ifndef __CC_TREE_NODE_EDGE_FA_H__
#define __CC_TREE_NODE_EDGE_FA_H__

#include <vector>
#include <array>

namespace ccsoft
{

class CC_TreeNodeEdge_FA_Tag_Empty
{
};

/**
 * \brief Represents a node and its incoming edge in the code tree
 * This version uses a fixed array to store forward node+edges pointers.
 * N_k template parameter gives the size of the input symbol (k parameter).
 * There are (1<<N_k) forward node+edges.
 * \tparam T_IOSymbol Type of the input and output symbols
 * \tparam T_Register Type of the encoder internal registers
 * \tparam T_Tag Type of the node-edge tag
 * \tparam N_k Input symbol size in bits (k parameter)
 */
template<typename T_IOSymbol, typename T_Register, typename T_Tag, unsigned int N_k>
class CC_TreeNodeEdge_FA
{

public:
    /**
     * Constructor
     * \param _id Unique ID of the edge
     * \param _p_incoming_edge Pointer to the incoming edge to the node
     * \param _in_symbol Input symbol corresponding to the edge
     * \param _metric Metric of the edge
     * \param _path_metric Path metric at the node
     * \param _depth This node depth
     */
	CC_TreeNodeEdge_FA(unsigned int _id,
			CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k> *_p_incoming_node_edge,
			const T_IOSymbol& _in_symbol,
            float _incoming_edge_metric,
            float _path_metric,
            int _depth) :
                id(_id),
                p_incoming_node_edge(_p_incoming_node_edge),
                in_symbol(_in_symbol),
                incoming_edge_metric(_incoming_edge_metric),
                path_metric(_path_metric),
                depth(_depth),
                on_final_path(false)
    {
	    p_outgoing_node_edges.fill(0);
    }

	/**
	 * Destructor
	 */
	~CC_TreeNodeEdge_FA()
	{
		// deletes all outgoing edge+nodes
		delete_outgoing_node_edges();
	}

    /**
     * Add an outgoing edge
     * \param p_outgoing_node_edge Outgoing edge+node
     */
    void set_outgoing_node_edge(CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k> *p_outgoing_node_edge, unsigned int index)
    {
    	p_outgoing_node_edges[index] = p_outgoing_node_edge;
    }

    /**
     * Delete outgoing edges
     */
    void delete_outgoing_node_edges()
    {
        typename std::array<CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k>*, (1<<N_k)>::iterator ne_it = p_outgoing_node_edges.begin();

        for (; ne_it != p_outgoing_node_edges.end(); ++ne_it)
        {
            if (*ne_it)
            {
                delete *ne_it;
                *ne_it = 0;
            }
        }

        p_outgoing_node_edges.fill(0);
    }

    /**
     * Verifies validity of outgoing node+edges i.e. pointers are all non null
     */
    bool valid_outgoing_node_edges(unsigned int index_limit = (1<<N_k))
    {
        for (unsigned int i=0; i<(1<<N_k); i++)
        {
            if (i == index_limit)
            {
                break;
            }

            if (p_outgoing_node_edges[i] == 0)
            {
                return false;
            }
        }

        return true;
    }

    /**
     * Return a R/O reference to the outgoing node+edges
     */
    const std::array<CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k>*, (1<<N_k)>& get_outgoing_node_edges() const
    {
        return p_outgoing_node_edges;
    }

    /**
     * Return a R/W reference to the outgoing edges
     */
    std::array<CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k>*, (1<<N_k)>& get_outgoing_node_edges()
    {
        return p_outgoing_node_edges;
    }

    /**
     * Get pointer to the incoming edge
     */
    CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k> *get_incoming_node_edge()
    {
        return p_incoming_node_edge;
    }

    /**
     * Get path metric to the node
     */
    float get_path_metric() const
    {
        return path_metric;
    }

    /**
     * Get the depth of the node
     */
    int get_depth() const
    {
        return depth;
    }

    /**
     * Get node id
     */
    unsigned int get_id() const
    {
        return id;
    }

    /**
     * For ordering by increasing path metric
     */
    bool operator<(const CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k>& other) const
    {
        return path_metric < other.path_metric;
    }

    /**
     * For ordering by decreasing path metric
     */
    bool operator>(const CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k>& other) const
    {
        return path_metric > other.path_metric;
    }

    /**
     * Get saved encoder registers reference
     */
    const std::array<T_Register, N_k>& get_registers() const
    {
        return registers;
    }

    /**
     * Save encoder registers
     */
    void set_registers(const std::array<T_Register, N_k>& _registers)
    {
        registers = _registers;
    }

    /**
     * Set the "on final path" marker
     */
    void set_on_final_path(bool _on_final_path = true)
    {
        on_final_path = _on_final_path;
    }

    /**
     * Test the "on final path" marker
     */
    bool is_on_final_path()
    {
        return on_final_path;
    }

    /**
     * Input symbol getter
     */
    const T_IOSymbol& get_in_symbol() const
    {
        return in_symbol;
    }

    /**
     * Incoming edge metric getter
     */
    float get_incoming_metric() const
    {
        return incoming_edge_metric;
    }

    /**
     * R/O reference to tag
     */
    const T_Tag& get_tag() const
    {
        return tag;
    }

    /**
     * R/W reference to tag
     */
    T_Tag& get_tag()
    {
        return tag;
    }

    /**
     * Ordering - lesser
     */
    bool operator<(const CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k>& other)
    {
        if (path_metric == other.path_metric)
        {
            return id < other.id;
        }
        else
        {
            return path_metric < other.path_metric;
        }
    }

    /**
     * Ordering - greater
     */
    bool operator>(const CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k>& other)
    {
        if (path_metric == other.path_metric)
        {
            return id > other.id;
        }
        else
        {
            return path_metric > other.path_metric;
        }
    }


protected:
    unsigned int id; //!< Node-edge's unique ID
    std::array<CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k>*, (1<<N_k)> p_outgoing_node_edges; //!< Outgoing edges+node pointers
    //std::vector<CC_TreeNodeEdge<T_IOSymbol, T_Register, T_Tag>*> p_outgoing_node_edges; //!< Outgoing edges+node pointers
    CC_TreeNodeEdge_FA<T_IOSymbol, T_Register, T_Tag, N_k> *p_incoming_node_edge; //!< Pointer to the incoming edge+node
    T_IOSymbol in_symbol; //!< Input symbol corresponding to the edge
    float path_metric; //!< Path metric to the node
    float incoming_edge_metric; //!< metric of the incoming edge to the node
    int depth; //!< Depth of node in the tree: 0 = root
    std::array<T_Register, N_k> registers; //!< state of encoder registers at node
    bool on_final_path; //!< Marks node when backtracking the solution
    T_Tag tag; //!< Optional and versatile object to tag the node+edge
};

} // namespace ccsoft

#endif