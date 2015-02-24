/*
 * K_nearest_builder.h
 *
 *  Created on: Sep 10, 2014
 *      Author: dsalinas
 */

#ifndef K_NEAREST_BUILDER_H_
#define K_NEAREST_BUILDER_H_

#include <unordered_map>
#include <boost/iterator/iterator_facade.hpp>
#include <CGAL/Euclidean_distance.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_d.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/property_map.h>
#include <boost/iterator/zip_iterator.hpp>

#include "utils/UI_utils.h"
#include "model/Complex_typedefs.h"



template<typename SkBlComplex> class K_nearest_builder{
private:

	typedef Geometry_trait Kernel;
	typedef Point Point_d;
	typedef boost::tuple<Point_d, unsigned> Point_d_with_id;
	typedef CGAL::Search_traits_d<Kernel> Traits_base;
	typedef CGAL::Search_traits_adapter<Point_d_with_id, CGAL::Nth_of_tuple_property_map<0, Point_d_with_id>, Traits_base> Traits;
	typedef CGAL::Orthogonal_k_neighbor_search<Traits> Neighbor_search;
	typedef Neighbor_search::Tree Tree;
	typedef Neighbor_search::Distance Distance;

	SkBlComplex& complex_;
public:

	/**
	 * @brief Modify complex to be the expansion of the k-nearest neighbor
	 * symetric graph.
	 */
	K_nearest_builder(SkBlComplex& complex,unsigned k):complex_(complex){
		complex.keep_only_vertices();
		compute_edges(k);
	}

private:



	double squared_eucl_distance(const Point& p1,const Point& p2) const{
		return Geometry_trait::Squared_distance_d()(p1,p2);
	}

	void compute_edges(unsigned k){

		std::list<Point_d_with_id> points_with_id;
		for(auto v: complex_.vertex_range()){
			Point_d_with_id point_v(complex_.point(v),v.vertex);
			points_with_id.push_back(point_v);
		}

		Tree tree(points_with_id.begin(),points_with_id.end());

		typedef typename SkBlComplex::Vertex_handle Vertex_handle;
		for (auto p : complex_.vertex_range()){
			Neighbor_search search(tree, complex_.point(p),k+1);
			for(auto it = ++search.begin(); it != search.end(); ++it){
				Vertex_handle q(boost::get<1>(it->first));
				if (p != q && complex_.contains_vertex(p) && complex_.contains_vertex(q))
					complex_.add_edge(p,q);
			}
		}
	}


};

#endif /* K_NEAREST_BUILDER_H_ */
