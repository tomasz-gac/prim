#ifndef __CHILDREN_ITERATORS_IMPL_HPP__
#define __CHILDREN_ITERATORS_IMPL_HPP__

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< std::reference_wrapper< Node<Ts...> >, N>::iterator
children_begin( node_impl__::CRTP::Static< Node< Ts... >, Derived, N >& node ){
  return node.children.begin();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< std::reference_wrapper< Node<Ts...> >, N>::iterator
children_end( node_impl__::CRTP::Static< Node< Ts... >, Derived, N >& node ){
  return node.children.end();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< std::reference_wrapper< Node<Ts...> >, N>::const_iterator
children_cbegin( const node_impl__::CRTP::Static< Node< Ts... >, Derived, N >& node ){
  return node.children.cbegin();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< std::reference_wrapper< Node<Ts...> >, N>::const_iterator
children_cend( const node_impl__::CRTP::Static< Node< Ts... >, Derived, N >& node ){
  return node.children.cend();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< std::reference_wrapper< Node<Ts...> >, N>::iterator
children_rbegin( node_impl__::CRTP::Static< Node< Ts... >, Derived, N >& node ){
  return node.children.rbegin();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< std::reference_wrapper< Node<Ts...> >, N>::iterator
children_rend( node_impl__::CRTP::Static< Node< Ts... >, Derived, N >& node ){
  return node.children.rend();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< std::reference_wrapper< Node<Ts...> >, N>::const_iterator
children_crbegin( const node_impl__::CRTP::Static< Node< Ts... >, Derived, N >& node ){
  return node.children.crbegin();
}

template< typename... Ts, typename Derived, size_t N > 
inline
typename std::array< std::reference_wrapper< Node<Ts...> >, N>::const_iterator
children_crend( const node_impl__::CRTP::Static< Node< Ts... >, Derived, N >& node ){
  return node.children.crend();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< std::reference_wrapper< Node< Ts... > > >::iterator
children_begin( node_impl__::CRTP::Dynamic< Node< Ts... >, Derived >& node ){
  return node.children.begin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< std::reference_wrapper< Node< Ts... > > >::iterator
children_end( node_impl__::CRTP::Dynamic< Node< Ts... >, Derived >& node ){
  return node.children.end();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< std::reference_wrapper< Node< Ts... > > >::const_iterator
children_cbegin( const node_impl__::CRTP::Dynamic< Node< Ts... >, Derived >& node ){
  return node.children.cbegin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< std::reference_wrapper< Node< Ts... > > >::const_iterator
children_cend( const node_impl__::CRTP::Dynamic< Node< Ts... >, Derived >& node ){
  return node.children.cend();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< std::reference_wrapper< Node< Ts... > > >::iterator
children_rbegin( node_impl__::CRTP::Dynamic< Node< Ts... >, Derived >& node ){
  return node.children.rbegin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< std::reference_wrapper< Node< Ts... > > >::iterator
children_rend( node_impl__::CRTP::Dynamic< Node< Ts... >, Derived >& node ){
  return node.children.rend();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< std::reference_wrapper< Node< Ts... > > >::const_iterator
children_crbegin( const node_impl__::CRTP::Dynamic< Node< Ts... >, Derived >& node ){
  return node.children.crbegin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< std::reference_wrapper< Node< Ts... > > >::const_iterator
children_crend( const node_impl__::CRTP::Dynamic< Node< Ts... >, Derived >& node ){
  return node.children.crend();
}

template< typename... Ts, typename Derived >
inline
Node<Ts...>* 
children_begin( node_impl__::CRTP::Terminal< Node<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
Node<Ts...>* 
children_end( node_impl__::CRTP::Terminal< Node<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const Node<Ts...>* 
children_cbegin( const node_impl__::CRTP::Terminal< Node<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const Node<Ts...>* 
children_cend( const node_impl__::CRTP::Terminal< Node<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
Node<Ts...>* 
children_rbegin( node_impl__::CRTP::Terminal< Node<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
Node<Ts...>* 
children_rend( node_impl__::CRTP::Terminal< Node<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const Node<Ts...>* 
children_crbegin( const node_impl__::CRTP::Terminal< Node<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const Node<Ts...>* 
children_crend( const node_impl__::CRTP::Terminal< Node<Ts...>, Derived >& ){
  return nullptr;
}


#endif // __CHILDREN_ITERATORS_IMPL_HPP__
