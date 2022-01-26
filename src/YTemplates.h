// template utilitiy classes.
//

#ifndef Y_TEMPLATES_H
#define Y_TEMPLATES_H

#include <map>

template<class _Key, class _Tp, class _Compare = less<_Key> >

class pointer_map : public map<_Key, void*, _Compare>
{
public:
  typedef map<_Key, void*, _Compare>  super_type;
  typedef _Key                        key_type;
  typedef _Tp                         mapped_type;
  typedef pair<const _Key, _Tp>       value_type;
  typedef _Compare                    key_compare;

#if 0
  class pointer : public super_type::rep_type::pointer
  {
    mapped_type* operator()(void)
      {return super_type::pointer::operator()();}
  };
  
  class const_pointer : public super_type::const_pointer
  {
    const mapped_type* operator()(void)
      {return super_type::const_pointer::operator()();}
  };
  
  class reference : public super_type::reference
  {
    mapped_type operator&(void)
      {return super_type::reference::operator&();}
  };
  
  class const_reference : public super_type::const_reference
  {
    const mapped_type operator&(void)
      {return super_type::const_reference::operator&();}
  };
#endif
  
  class iterator
  {
  public:
    typedef iterator _Self;
    iterator (void) {;};
    iterator (const super_type::iterator &_s) : s(_s) {;};
    iterator (const iterator &_i) : s(_i.s) {;};
    value_type *operator->(void) {return (value_type*)&(*s);}
    value_type &operator*(void) {return *(value_type*)&(*s);}
    iterator &operator++(void) {++s; return *this;}
    iterator operator++(int) {iterator t = *this; ++s; return t;}
    iterator &operator=(const iterator& _i) {s = _i.s; return *this;}

    bool operator==(const iterator &_i) {return s == _i.s;}
    bool operator!=(const iterator &_i) {return s != _i.s;}

    super_type::iterator s;
  };

#if 0  
  class const_iterator : public super_type::const_iterator
  {
    const mapped_type operator*(void)
      {return super_type::const_iterator::operator*();}
  };
  
  class reverse_iterator : public super_type::reverse_iterator
  {
    mapped_type operator*(void)
      {return super_type::reverse_iterator::operator*();}
  };
  
  class const_reverse_iterator : public super_type::const_reverse_iterator
  {
    const mapped_type operator*(void)
      {return super_type::const_reverse_iterator::operator*();}
  };

  typedef typename super_type::size_type       size_type;
  typedef typename super_type::difference_type difference_type;
  typedef typename super_type::allocator_type  allocator_type;
#endif

  iterator begin(void) 
    {return iterator(super_type::begin());}
  iterator end(void) 
  {return iterator(super_type::end());}

  mapped_type &operator[](const _Key &_k)
    {return *(mapped_type*)&(super_type::operator[](_k));}
  
};

#endif
