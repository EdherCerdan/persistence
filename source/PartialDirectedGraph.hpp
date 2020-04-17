// Copyright 2020 Roger Peralta Aranibar
#ifndef SOURCE_PARTIALDIRECTEDGRAPH_HPP_
#define SOURCE_PARTIALDIRECTEDGRAPH_HPP_

#include <utility>

#include "DirectedGraph.hpp"

namespace ADE {
namespace Persistence {
template <typename Type>
class Table{
public:
  typedef Type data_type;
  int version;
  int table_type;  // 1: modificacion de valor, 2: modificacion de puntero
  Node<Type> *ptr;
  data_type data;
  Table(int type, int ver) {
  	ptr = nullptr;
  	data = 100;
  	table_type = type;
  	version = ver;
  }
};

template <typename Type>
class PartialNode : public Node<Type> {
 public:
  typedef Type data_type;
  PartialNode(data_type data, std::size_t const& out_ptrs_size)
      : Node<Type>(data, out_ptrs_size) {table = nullptr;}

  PartialNode& operator[](
      std::pair<std::size_t, unsigned int> id_version) const {
    Node<Type> *next_node_ptr = nullptr;
    if(table){
    	if (table->version == id_version.second){
    		next_node_ptr = table->ptr;
    		return *dynamic_cast<PartialNode*>(next_node_ptr);
    	}
    }
    	if (version == id_version.second){
  	    	return *dynamic_cast<PartialNode*>(&(Node<Type>::operator[](id_version.first)));
    	}
  }
  Table<Type> *table;
  void create_table(int type, int ver){
  	table = new Table<Type>(type, ver);
  }
  int version;

};

template <typename Type, typename Node = PartialNode<Type>>
class PartialDirectedGraph : public DirectedGraph<Type, Node> {
 public:
  typedef Type data_type;

  PartialDirectedGraph(data_type const data, std::size_t const& out_ptrs_size,
                       std::size_t const& in_ptrs_size)
      : DirectedGraph<Type, Node>(data, out_ptrs_size),
        in_ptrs_size_(in_ptrs_size),
        current_version(0) {}

  Node* get_root_ptr(unsigned int version) {
    return DirectedGraph<Type, Node>::get_root_ptr();
  }

  Node* insert_vertex(data_type const data, Node* u, std::size_t position) {
    ++current_version;
    //std::cout<<"Insert vertex: current version: "<<current_version<<std::endl;
    return insert_vertex(data, u, position, current_version);
  }

  void add_edge(Node* u, Node* v, std::size_t position) {
    // ++current_version;
    // std::cout<<"Add edge: current version: "<<current_version<<std::endl;
    return add_edge(u, v, position, current_version);
  }

 private:	
  Node* insert_vertex(data_type const data, Node* u, std::size_t position,
                      unsigned int version) {
  	Node* next_node_ptr = dynamic_cast<Node*>(u->forward_[position]);
    Node* inserted_node;
  	if (next_node_ptr){
  		if (u->table != 0){
  			std::cout<<"SE DEBE CREAR UN NODO NUEVO:"<<version<<std::endl;
  		}
  		else{
  			u->create_table(2, version);  			
  			Node* next_node_ptr = dynamic_cast<Node*>(u->forward_[position]);
            Node* new_node = new Node(data, u->out_ptrs_size_);
            new_node->forward_[position] = next_node_ptr;
            new_node->version=version;
            u->table->ptr= new_node;
            return new_node;
  		}
  	}
  	else{
  		inserted_node =
        DirectedGraph<Type, Node>::insert_vertex(data, u, position);
        inserted_node->version = version;
    	Node* inserted_by_forward = dynamic_cast<Node*>(u->forward_[position]);
        return inserted_node;


  	}
        /*DirectedGraph<Type, Node>::insert_vertex(data, u, position);
    Node* inserted_by_forward = dynamic_cast<Node*>(u->forward_[position]);
    std::cout << "inserted forward: " << *inserted_by_forward->data_
              << std::endl;*/
    return inserted_node;
  }

  void add_edge(Node* u, Node* v, std::size_t position, unsigned int version) {
    return DirectedGraph<Type, Node>::add_edge(u, v, position);
  }

  std::size_t in_ptrs_size_;
  unsigned int current_version;
};

}  // namespace Persistence
}  // namespace ADE

#endif  // SOURCE_PARTIALDIRECTEDGRAPH_HPP_