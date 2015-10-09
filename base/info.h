/*
   Copyright (c) 2003, Adrian Rossiter

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

      The above copyright notice and this permission notice shall be included
      in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/

/*
   Name: info.h
   Description: information from OFF files
   Project: Antiprism - http://www.antiprism.com
*/


#ifndef INFO_H
#define INFO_H

#include "geom.h"
#include "transforms.h"
using std::pair;


int cmp_angles(const double &a, const double &b, double min_diff=1e-6);
int cmp_face_angles(const vector<double> &f1, const vector<double> &f2, double min_diff=1e-6);

class ang_less {
   public:
      bool operator() (const double &a, const double &b) const {
         return (cmp_angles(a, b) < 0);
      }
};

class ang_vect_less {
   public:
      bool operator() (const vector<double> &f1,
            const vector<double> &f2) const {
         if(f1.size() != f2.size())
            return (f1.size() < f2.size());
         return (cmp_face_angles(f1, f2) < 0);
      }
};


class elem_lims
{
   public:
      enum { IDX_MIN=0, IDX_MIN2, IDX_MAX, IDX_MAX2, IDX_MID, IDX_MID2 };
      
      int idx[6];
      double max;
      double min;
      double mid;
      double sum;
      
      elem_lims() { init(); }
      void init() { idx[0]=-1; max=-1e100; min=1e100; mid=1e100; sum=0; }
      bool is_set() const { return idx[0] != -1; }
};
   
class elem_distances : public elem_lims
{
   protected:
      const geom_if &geom;
      vec3d center;
   public:

      elem_distances(const geom_if &geo): geom(geo) { set_center();}
      virtual ~elem_distances() {}
      void set_center(vec3d cent=vec3d(0,0,0)) {center = cent; idx[0] = -1; }
      virtual void set_values() = 0;
};

class v_distances: public elem_distances
{ 
   public:
      v_distances(const geom_if &geom): elem_distances(geom) { idx[0] = -1; }
      void set_values();
};

class e_distances: public elem_distances
{ 
   public:
      e_distances(const geom_if &geom): elem_distances(geom) { idx[0] = -1; }
      void set_values();
};

class ie_distances: public elem_distances
{
   public:
      ie_distances(const geom_if &geom): elem_distances(geom) { idx[0] = -1; }
      void set_values();
};

class f_distances: public elem_distances
{ 
   public:
      f_distances(const geom_if &geom): elem_distances(geom) { idx[0] = -1; }
      void set_values();
};

struct double_range_cnt
{
   int cnt;
   double min;
   double max;

   double_range_cnt(): cnt(0), min(1e100), max(-1e100) {}
   double_range_cnt update(double val)
      { cnt++; if(val<min) min=val; if(val>max) max=val; return *this; }
   double mid() const { return (min+max)/2; }  // middle of range
   double rad() const { return (max-min)/2; }  // radius of range
};



class geom_info
{
   private:
      vec3d cent;
      int oriented;
      int orientable;
      bool found_connectivity;
      bool closed;
      bool polyhedron;
      bool known_connectivity;
      bool even_connectivity;
      int number_parts;
      int genus_val;
      elem_lims iedge_len;
      elem_lims edge_len;
      elem_lims so_angles;
      elem_lims dih_angles;
      elem_lims ang;
      int num_angs;
      elem_lims area;
      double vol;
      vec3d vol_cent;
      v_distances v_dsts;
      e_distances e_dsts;
      ie_distances ie_dsts;
      f_distances f_dsts;

      vector<vector<int> > impl_edges;
      map<vector<int>, vector<int> > efpairs;
      vector<vector<int> > edge_parts;
      map<vector<double>, int, ang_vect_less> face_angles;
      map<vector<double>, int, ang_vect_less> vert_dihed;
      map<double, double_range_cnt, ang_less> dihedral_angles;
      map<double, double_range_cnt, ang_less> e_lengths;
      map<double, double_range_cnt, ang_less> ie_lengths;
      map<double, double_range_cnt, ang_less> plane_angles;
      map<double, double_range_cnt, ang_less> sol_angles;
      vector<double> vertex_angles;
      map<pair<int, int>, double> vertex_plane_angs;
      vector<double> edge_dihedrals;
      vector<double> f_areas;
      vector<double> f_perimeters;
      vector<double> f_max_nonplanars;
      vector<vector<int> > vert_cons;
      vector<vector<int> > vert_cons_orig;
      vector<vector<vector<int> > > vert_figs;
      vector<vec3d> vert_norms;
      bool vert_norms_local_orient;
      vector<int> free_verts;
      bool found_free_verts;
      geom_v dual;
      sch_sym sym;
   
      void find_impl_edges() { geom.get_impl_edges(impl_edges); }
      void find_edge_face_pairs();
      void find_edge_parts();
      void find_connectivity();
      void find_face_angles();
      void find_dihedral_angles();
      void find_vert_cons();
      void find_vert_cons_orig();
      void find_vert_figs();
      void find_vert_norms(bool raw_orientation=false);
      void find_free_verts();
      void find_solid_angles();
      void find_e_lengths(map<double, double_range_cnt, ang_less> &e_lens,
         const vector<vector<int> > &edges, elem_lims &lens);
      void find_e_lengths();
      void find_f_areas();
      void find_f_perimeters();
      void find_f_max_nonplanars();
      void find_oriented();
      void find_symmetry();

   protected:   
      const geom_if &geom;

   public:
      geom_info(const geom_if &geo) :
         cent(vec3d(0,0,0)), oriented(-1), orientable(-1),
         found_connectivity(false), genus_val(INT_MAX),
         v_dsts(geo), e_dsts(geo), ie_dsts(geo), f_dsts(geo),
         found_free_verts(false), geom(geo)
         {}
      
      void reset();
      void set_center(vec3d center) { cent = center; v_dsts.set_center(cent);
         e_dsts.set_center(cent); ie_dsts.set_center(cent);
         f_dsts.set_center(cent); }

      // elements
      double face_area(int f_no);
      double face_vol(int f_no, vec3d *face_vol_cent);
      void face_angles_lengths(int f_no, vector<double> &angs,
            vector<double> *lens=0);

      bool is_oriented();
      bool is_orientable();
      bool is_closed();
      bool is_polyhedron();
      bool is_even_connectivity();
      bool is_known_connectivity();

      int num_verts() { return geom.verts().size(); }
      int num_edges() { return geom.edges().size(); }
      int num_iedges() { return get_impl_edges().size(); }
      int num_faces() { return geom.faces().size(); }
      int num_parts() { is_orientable(); return number_parts; }
      int num_edge_parts() { find_edge_parts(); return edge_parts.size(); }

      bool is_known_genus();
      int genus();

      elem_lims face_areas() { if(f_areas.size()==0) find_f_areas();
                           return area; }
      double volume() { if(f_areas.size()==0) find_f_areas(); return vol;}
      vec3d volume_centroid()
         { if(f_areas.size()==0) find_f_areas(); return vol_cent;}
      double isoperimetric_quotient()
         { return 36.0*M_PI*pow(volume(),2)/pow(face_areas().sum,3); }

      elem_lims edge_lengths() { get_e_lengths(); return edge_len; }
      elem_lims iedge_lengths() { get_ie_lengths(); return iedge_len; }
      elem_lims dihed_angles()
                  { if(dihedral_angles.size()==0) find_dihedral_angles();
                    return dih_angles; }
      elem_lims solid_angles() {if(sol_angles.size()==0) find_solid_angles();
                                return so_angles; }
      vec3d center() { return cent; }
      elem_distances &vert_dists()
         { if(!v_dsts.is_set()) v_dsts.set_values(); return v_dsts; }
      elem_distances &edge_dists()
         { if(!e_dsts.is_set()) e_dsts.set_values(); return e_dsts; }
      elem_distances &impl_edge_dists()
         { if(!ie_dsts.is_set()) ie_dsts.set_values(); return ie_dsts; }
      elem_distances &face_dists()
         { if(!f_dsts.is_set()) f_dsts.set_values(); return f_dsts; }
      elem_lims angles()
         { if(!plane_angles.size()) find_face_angles(); return ang; }
      int num_angles()
         { if(!plane_angles.size()) find_face_angles(); return num_angs; }
      double angle_defect() { return num_verts()*2*M_PI - angles().sum; }

      //verts
      const vector<vec3d> &get_vert_norms(bool local_orient=true)
         { if(!vert_norms.size() || local_orient != vert_norms_local_orient)
            find_vert_norms(local_orient); return vert_norms; }
      const vector<vector<int> > &get_vert_cons()
         { if(!vert_cons.size()) find_vert_cons(); return vert_cons; }
      const vector<vector<int> > &get_vert_cons_orig()
         { if(!vert_cons_orig.size()) find_vert_cons_orig(); return vert_cons_orig; }
      const vector<vector<vector<int> > > &get_vert_figs()
         { if(!vert_figs.size()) find_vert_figs(); return vert_figs; }
      const vector<double> &get_vertex_angles()
         { if(!vertex_angles.size()) find_solid_angles(); return vertex_angles;}
      map<double, double_range_cnt, ang_less> &get_solid_angles()
         { if(!sol_angles.size()) find_solid_angles(); return sol_angles; }
      map<pair<int, int>, double> &get_vertex_plane_angs()
         { if(!vertex_plane_angs.size()) find_face_angles();
            return vertex_plane_angs; }
      const vector<int> &get_free_verts()
         { if(!found_free_verts) find_free_verts(); return free_verts;}
      
      //edges
      map<vector<int>, vector<int> > &get_edge_face_pairs()
         { if(!efpairs.size()) find_edge_face_pairs(); return efpairs; }
      vector<double> &get_edge_dihedrals()
         { if(!dihedral_angles.size()) find_dihedral_angles();
            return edge_dihedrals; }
      vector<vector<int> > &get_edge_parts()
         { if(!edge_parts.size()) find_edge_parts();
            return edge_parts; }
      map<double, double_range_cnt, ang_less> &get_dihedral_angles()
         { if(!dihedral_angles.size()) find_dihedral_angles();
            return dihedral_angles; }
      map<double, double_range_cnt, ang_less> &get_e_lengths()
         { if(!e_lengths.size())
              find_e_lengths(e_lengths, geom.edges(), edge_len);
           return e_lengths; }
      
      //implicit edges
      vector<vector<int> > &get_impl_edges()
         { if(!impl_edges.size()) find_impl_edges(); return impl_edges; }
      map<double, double_range_cnt, ang_less> &get_ie_lengths()
         { if(!ie_lengths.size())
              find_e_lengths(ie_lengths, get_impl_edges(), iedge_len);
           return ie_lengths; }
      
      //faces
      map<vector<double>, int, ang_vect_less> &get_face_angles()
         { if(!face_angles.size()) find_face_angles(); return face_angles; }
      vector<double> &get_f_areas()
         { if(!f_areas.size()) find_f_areas(); return f_areas; }
      vector<double> &get_f_perimeters()
         { if(!f_perimeters.size()) find_f_perimeters(); return f_perimeters; }
      vector<double> &get_f_max_nonplanars()
         { if(!f_max_nonplanars.size()) find_f_max_nonplanars();
           return f_max_nonplanars; }
      geom_v &get_dual()
         { if(!dual.get_faces()->size()) ::get_dual(geom, dual); return dual; } 

      const geom_if &get_geom() const { return geom; }
  
      //Symmetry
      const sch_sym &get_symmetry()
         { if(!sym.is_set()) find_symmetry(); return sym; }
      string get_symmetry_type_name()
         { if(!sym.is_set()) find_symmetry(); return sym.get_symbol(); }
      const set<sch_axis> &get_symmetry_axes()
         { if(!sym.is_set()) find_symmetry(); return sym.get_axes(); }
      const set<sch_sym> &get_symmetry_subgroups()
         { if(!sym.is_set()) find_symmetry(); return sym.get_sub_syms(); }
      const sch_sym_autos &get_symmetry_autos()
         { if(!sym.is_set()) find_symmetry(); return sym.get_autos(); }
      mat3d get_symmetry_alignment_to_std()
         { if(!sym.is_set()) find_symmetry(); return sym.get_to_std(); }
};


#endif // INFO_H
