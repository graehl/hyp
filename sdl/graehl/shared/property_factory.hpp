






  G g;
  typedef typename P::rebind<value> F;





  reminder: boost property maps (namespace usually found by ADL on map type):

  put(map, key, val)
  get(map, key)

*/

#ifndef GRAEHL_SHARED__PROPERTY_FACTORY_HPP
#define GRAEHL_SHARED__PROPERTY_FACTORY_HPP



#include <graehl/shared/graph.hpp>
#include <boost/property_map/shared_array_property_map.hpp>  // new A[n]
#include <graehl/shared/word_spacer.hpp>
#include <boost/range/adaptor/transformed.hpp>

namespace graehl {

// and others if you like!

// size_traits - for default property_factory, which assumes ids [0..size)

template <class G, class ptag>
struct size_traits {
  // typedef std::size_t size_type;
};

template <class G>
struct size_traits<G, vertex_tag> {
  typedef typename boost::graph_traits<G>::vertices_size_type size_type;
};

template <class G>
struct size_traits<G, edge_tag> {
  typedef typename boost::graph_traits<G>::edges_size_type size_type;
};

template <class G>
typename size_traits<G, vertex_tag>::size_type size(G const& g, vertex_tag) {
  using namespace boost;
  return num_vertices(g);
}

template <class G>
typename size_traits<G, edge_tag>::edge_size_type size(G const& g, edge_tag) {
  using namespace boost;
  return num_edges(g);
}


struct identity_offset_map {
  typedef std::size_t key_type;
  typedef std::size_t value_type;
  typedef value_type const& reference;
  typedef boost::readable_property_map_tag category;
  template <class I>
  reference operator[](I const& i) const {
    return i;
  }
};


  return i;
}


}  // ns


namespace graehl {












// specialize, unless you have integral [0...size) descriptors
template <class G, class ptag>
struct property_factory {
 public:
  typedef typename size_traits<G, ptag>::size_type init_type;
  init_type N;

  template <class V>
  init_type init() const {
    return N;
  }
  template <class V>
  struct rebind {
    property_factory const& p;
    rebind(property_factory const& p) : p(p) {}
    rebind(rebind const& o) : p(o.p) {}
    init_type init() const { return p.template init<V>(); }

    typedef reference impl;
  };




};

template <class V, class G, class ptag>


  return i;
}

template <class P>
struct pmap_getter {
  P const& p;
  explicit pmap_getter(P const& p) : p(p) {}
  typedef boost::property_traits<P> T;
  typedef void key_type;
  typedef typename T::value_type const& result_type;
  template <class key_type>
  result_type operator()(key_type const& k) const {
    return get(p, k);
  }
};

template <class P>
pmap_getter<P> getter(P const& p) {
  return pmap_getter<P>(p);
}

template <class K, class V>
struct map_pair {
  K k;  // unsure if can make these refs in boost transform chain, so copy the val.
  V v;
  map_pair(K const& k, V const& v) : k(k), v(v) {}
  typedef map_pair self_type;
  template <class O>
  void print(O& o) const {
    o << k << " -> " << v;
  }
  TO_OSTREAM_PRINT
};

template <class K, class V>
map_pair<K, V> mpair(K const& k, V const& v) {
  return map_pair<K, V>(k, v);
}

template <class A, class P>
struct pmap_pair_getter {
  P const& p;
  explicit pmap_pair_getter(P const& p) : p(p) {}
  typedef boost::property_traits<P> T;
  typedef A key_type;
  typedef typename T::value_type V;
  typedef map_pair<key_type, V> result_type;
  // template <class key_type>

};

template <class A, class P>
pmap_pair_getter<A, P> pair_getter_argtype(P const& p) {
  return pmap_pair_getter<A, P>(p);
}

template <class P>
pmap_pair_getter<typename boost::property_traits<P>::key_type, P> pair_getter(P const& p) {
  return pmap_pair_getter<typename boost::property_traits<P>::key_type, P>(p);
}

template <class O, class R, class P>
void print(O& o, R const& r, pmap_getter<P> const& p) {
  range_sep(multiline).print(o, r | boost::adaptors::transformed(p));
}

template <class O, class R, class A, class P>
void print(O& o, R const& r, pmap_pair_getter<A, P> const& p) {
  range_sep(multiline).print(o, r | boost::adaptors::transformed(p));
}

/* like static_property_map but you can "put" to it (val ignored)
 */
template <typename V>
class const_sink_property_map
    //: public boost::put_get_helper<V, const_sink_property_map<V> >
    {
  V v;

 public:
  typedef void key_type;
  typedef V value_type;
  typedef V reference;
  typedef boost::read_write_property_map_tag category;
  explicit const_sink_property_map(V v = V()) : v(v) {}

  template <typename T>




  template <typename T>
  friend inline value_type get(const_sink_property_map const& m, T) {
    return m.v;
  }
  template <typename T>

};

// values are ALWAYS default constructed on first get.

template <class V>
const_sink_property_map<V> const_sink_pmap(V const& v) {
  return const_sink_property_map<V>(v);
}

template <class P, class V>
struct set_pmap {
  P& p;
  V const& v;
  set_pmap(P& p, V const& v) : p(p), v(v) {}
  template <class I>
  void operator()(I const& i) const {
    put(p, i, v);
  }
};

template <class P, class V>




template <class G, class P, class T, class V>
void init_pmap(T t, G const& g, P& p, V const& v = V()) {
  visit(t, g, pmap_setter(p, v));
}

template <class G, class T, class V>


  return reference(*pimpl);
}

template <class T, class G, class V>
struct built_pmap {
  typedef property_factory<G, T> P;
  typedef typename P::template rebind<V> R;
  typedef typename R::impl I;
  typedef typename R::reference property_map_type;
  G const& g;
  I impl;
  property_map_type pmap;
  built_pmap(G const& g) : g(g), impl(P(g).template init<V>()), pmap(impl) {}


};

template <class D, class S>
struct copy_pmap_v {
  D d;
  S s;
  copy_pmap_v(D d, S s) : d(d), s(s) {}
  template <class I>
  void operator()(I const& i) const {
    put(d, i, get(s, i));
  }
};

template <class T, class G, class D, class S>
void copy_pmap(T t, G const& g, D const& dest, S const& src) {
  visit(t, g, copy_pmap_v<D, S>(dest, src));
}

template <class C1, class C2, class C3>
struct IndexedPairCopier {



  IndexedPairCopier(C1 const& a, C2 const& b, C3 const& c) : a(a), b(b), c(c) {}
  typedef typename boost::property_traits<C2>::key_type K;
  void operator()(K const& k) const {
    using namespace boost;
    typedef typename property_traits<C1>::value_type Pair;
#if 1
    Pair& p = a[k];
    p.first = get(b, k);
    p.second = get(c, k);
#else
    put(a, k, Pair(get(b, k), get(c, k)));
#endif
  }
};

template <class C1, class C2, class C3>
IndexedPairCopier<C1, C2, C3> make_indexed_pair_copier(C1 const& a, C2 const& b, C3 const& c) {
  return IndexedPairCopier<C1, C2, C3>(a, b, c);
}




#endif