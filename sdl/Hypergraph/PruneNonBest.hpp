





























































  MutableHypergraph<A> const* phg;

  StateId start;


  ArcInBest(IHypergraph<A> const& hg, PruneNonBestOptions const& opt) :
      phg(&dynamic_cast<MutableHypergraph<A> const&>(hg)),
      pi(ComputeBest(BestPathOptions(), hg).predecessors()),

  {







































      }
    }












































