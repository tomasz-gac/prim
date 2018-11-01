#ifndef __TRACKER_HPP__
#define __TRACKER_HPP__

class Counter{
private:
  std::size_t count_;

public:
  std::size_t count() const{ return count_; }
  void inc(){ ++count_; }
  void dec(){ --count_; }
  void reset(){ count_ = 0; }

  Counter( std::size_t c = 0 ) : count_(c){  }
  Counter( const Counter&  other ) : Counter(other.count()) {  }
};

template< typename T >
class Guard;

struct Tracker{
  Counter objects, copies, moves;

  template< typename T, typename... Args >
  Guard<typename std::remove_reference<T>::type> track( Args&&... args ){
    return { *this, std::forward<Args>(args)... };
  }
};

template< typename T >
class Guard{
private:
  Tracker* tracker;

public:
  T value;

  operator T&(){ return value; }
  operator const T&() const { return value; }

  template< typename... Args >
  Guard( Tracker& t, Args&&... args )
    : tracker( &t )
    , value( std::forward<Args>(args)... )
  { tracker->objects.inc();  }
  
  Guard( const Guard& other )
    : tracker( other.tracker )
    , value( other.value )
  {
    tracker->objects.inc();
    tracker->copies.inc();
    // std::cout << "copy" << std::endl;     
  }

  Guard( Guard&& other )
    noexcept( std::is_nothrow_move_constructible<T>::value )
    : tracker( other.tracker )
    , value( std::move(other.value) )
  {
    tracker->objects.inc();
    tracker->moves.inc();
    // std::cout << "move" << std::endl;     
  }

  Guard& operator=( const Guard& other ){
    tracker = other.tracker;
    value = other.value;
    tracker->copies.inc();
    return *this;
  }

  Guard& operator=( Guard&& other )
    noexcept( std::is_nothrow_move_assignable<T>::value )
  {
    tracker = other.tracker;
    value = std::move(other.value);
    tracker->moves.inc();
    return *this;
  }

  ~Guard(){
    tracker->objects.dec();
    // std::cout << "destroy" << std::endl;
  }
};

std::ostream& operator<<( std::ostream& str, const Tracker& t ){
  str << "Objects : " << t.objects.count() << " Copies : " << t.copies.count() << " Moves : " << t.moves.count();
  return str;
}

#endif // __TRACKER_HPP__
