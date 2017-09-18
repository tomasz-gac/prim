#ifndef __CHILDREN_ITERATORS_IMPL_HPP__
#define __CHILDREN_ITERATORS_IMPL_HPP__

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Node< INode< Ts... > >, N>::iterator
children_begin( node_impl__::CRTP::Static< INode< Ts... >, Derived, N >& node ){
  return node.children.begin();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Node< INode< Ts... > >, N>::iterator
children_end( node_impl__::CRTP::Static< INode< Ts... >, Derived, N >& node ){
  return node.children.end();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Node< INode< Ts... > >, N>::const_iterator
children_cbegin( const node_impl__::CRTP::Static< INode< Ts... >, Derived, N >& node ){
  return node.children.cbegin();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Node< INode< Ts... > >, N>::const_iterator
children_cend( const node_impl__::CRTP::Static< INode< Ts... >, Derived, N >& node ){
  return node.children.cend();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Node< INode< Ts... > >, N>::iterator
children_rbegin( node_impl__::CRTP::Static< INode< Ts... >, Derived, N >& node ){
  return node.children.rbegin();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Node< INode< Ts... > >, N>::iterator
children_rend( node_impl__::CRTP::Static< INode< Ts... >, Derived, N >& node ){
  return node.children.rend();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Node< INode< Ts... > >, N>::const_iterator
children_crbegin( const node_impl__::CRTP::Static< INode< Ts... >, Derived, N >& node ){
  return node.children.crbegin();
}

template< typename... Ts, typename Derived, size_t N > 
inline
typename std::array< Node< INode< Ts... > >, N>::const_iterator
children_crend( const node_impl__::CRTP::Static< INode< Ts... >, Derived, N >& node ){
  return node.children.crend();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Node< INode< Ts... > > >::iterator
children_begin( node_impl__::CRTP::Dynamic< INode< Ts... >, Derived >& node ){
  return node.children.begin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Node< INode< Ts... > > >::iterator
children_end( node_impl__::CRTP::Dynamic< INode< Ts... >, Derived >& node ){
  return node.children.end();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Node< INode< Ts... > > >::const_iterator
children_cbegin( const node_impl__::CRTP::Dynamic< INode< Ts... >, Derived >& node ){
  return node.children.cbegin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Node< INode< Ts... > > >::const_iterator
children_cend( const node_impl__::CRTP::Dynamic< INode< Ts... >, Derived >& node ){
  return node.children.cend();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Node< INode< Ts... > > >::iterator
children_rbegin( node_impl__::CRTP::Dynamic< INode< Ts... >, Derived >& node ){
  return node.children.rbegin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Node< INode< Ts... > > >::iterator
children_rend( node_impl__::CRTP::Dynamic< INode< Ts... >, Derived >& node ){
  return node.children.rend();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Node< INode< Ts... > > >::const_iterator
children_crbegin( const node_impl__::CRTP::Dynamic< INode< Ts... >, Derived >& node ){
  return node.children.crbegin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Node< INode< Ts... > > >::const_iterator
children_crend( const node_impl__::CRTP::Dynamic< INode< Ts... >, Derived >& node ){
  return node.children.crend();
}

template< typename... Ts, typename Derived >
inline
INode<Ts...>* 
children_begin( node_impl__::CRTP::Terminal< INode<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
INode<Ts...>* 
children_end( node_impl__::CRTP::Terminal< INode<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const INode<Ts...>* 
children_cbegin( const node_impl__::CRTP::Terminal< INode<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const INode<Ts...>* 
children_cend( const node_impl__::CRTP::Terminal< INode<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
INode<Ts...>* 
children_rbegin( node_impl__::CRTP::Terminal< INode<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
INode<Ts...>* 
children_rend( node_impl__::CRTP::Terminal< INode<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const INode<Ts...>* 
children_crbegin( const node_impl__::CRTP::Terminal< INode<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const INode<Ts...>* 
children_crend( const node_impl__::CRTP::Terminal< INode<Ts...>, Derived >& ){
  return nullptr;
}


#endif // __CHILDREN_ITERATORS_IMPL_HPP__
