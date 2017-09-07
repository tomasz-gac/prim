#ifndef __CHILDREN_ITERATORS_IMPL_HPP__
#define __CHILDREN_ITERATORS_IMPL_HPP__

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Tree<Ts...>, N>::iterator
children_begin( tree_impl__::CRTP::Static< Tree< Ts... >, Derived, N >& node ){
  return node.children.begin();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Tree<Ts...>, N>::iterator
children_end( tree_impl__::CRTP::Static< Tree< Ts... >, Derived, N >& node ){
  return node.children.end();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Tree<Ts...>, N>::const_iterator
children_cbegin( const tree_impl__::CRTP::Static< Tree< Ts... >, Derived, N >& node ){
  return node.children.cbegin();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Tree<Ts...>, N>::const_iterator
children_cend( const tree_impl__::CRTP::Static< Tree< Ts... >, Derived, N >& node ){
  return node.children.cend();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Tree<Ts...>, N>::iterator
children_rbegin( tree_impl__::CRTP::Static< Tree< Ts... >, Derived, N >& node ){
  return node.children.rbegin();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Tree<Ts...>, N>::iterator
children_rend( tree_impl__::CRTP::Static< Tree< Ts... >, Derived, N >& node ){
  return node.children.rend();
}

template< typename... Ts, typename Derived, size_t N >
inline
typename std::array< Tree<Ts...>, N>::const_iterator
children_crbegin( const tree_impl__::CRTP::Static< Tree< Ts... >, Derived, N >& node ){
  return node.children.crbegin();
}

template< typename... Ts, typename Derived, size_t N > 
inline
typename std::array< Tree<Ts...>, N>::const_iterator
children_crend( const tree_impl__::CRTP::Static< Tree< Ts... >, Derived, N >& node ){
  return node.children.crend();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Tree< Ts... > >::iterator
children_begin( tree_impl__::CRTP::Dynamic< Tree< Ts... >, Derived >& node ){
  return node.children.begin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Tree< Ts... > >::iterator
children_end( tree_impl__::CRTP::Dynamic< Tree< Ts... >, Derived >& node ){
  return node.children.end();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Tree< Ts... > >::const_iterator
children_cbegin( const tree_impl__::CRTP::Dynamic< Tree< Ts... >, Derived >& node ){
  return node.children.cbegin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Tree< Ts... > >::const_iterator
children_cend( const tree_impl__::CRTP::Dynamic< Tree< Ts... >, Derived >& node ){
  return node.children.cend();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Tree< Ts... > >::iterator
children_rbegin( tree_impl__::CRTP::Dynamic< Tree< Ts... >, Derived >& node ){
  return node.children.rbegin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Tree< Ts... > >::iterator
children_rend( tree_impl__::CRTP::Dynamic< Tree< Ts... >, Derived >& node ){
  return node.children.rend();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Tree< Ts... > >::const_iterator
children_crbegin( const tree_impl__::CRTP::Dynamic< Tree< Ts... >, Derived >& node ){
  return node.children.crbegin();
}

template< typename... Ts, typename Derived >
inline
typename std::vector< Tree< Ts... > >::const_iterator
children_crend( const tree_impl__::CRTP::Dynamic< Tree< Ts... >, Derived >& node ){
  return node.children.crend();
}

template< typename... Ts, typename Derived >
inline
Tree<Ts...>* 
children_begin( tree_impl__::CRTP::Terminal< Tree<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
Tree<Ts...>* 
children_end( tree_impl__::CRTP::Terminal< Tree<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const Tree<Ts...>* 
children_cbegin( const tree_impl__::CRTP::Terminal< Tree<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const Tree<Ts...>* 
children_cend( const tree_impl__::CRTP::Terminal< Tree<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
Tree<Ts...>* 
children_rbegin( tree_impl__::CRTP::Terminal< Tree<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
Tree<Ts...>* 
children_rend( tree_impl__::CRTP::Terminal< Tree<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const Tree<Ts...>* 
children_crbegin( const tree_impl__::CRTP::Terminal< Tree<Ts...>, Derived >& ){
  return nullptr;
}

template< typename... Ts, typename Derived >
inline
const Tree<Ts...>* 
children_crend( const tree_impl__::CRTP::Terminal< Tree<Ts...>, Derived >& ){
  return nullptr;
}


#endif // __CHILDREN_ITERATORS_IMPL_HPP__
