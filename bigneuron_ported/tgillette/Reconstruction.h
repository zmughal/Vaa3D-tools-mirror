//
//  Reconstruction.h
//  BigNeuron Consensus Builder
//
//  Contains class definitions for the Composite and related objects, including wrappers for reconstruction elements
//
//  Created by Todd Gillette on 5/27/15.
//  Copyright (c) 2015 Todd Gillette. All rights reserved.
//

#ifndef BigNeuron_Consensus_Builder_Reconstruction_h
#define BigNeuron_Consensus_Builder_Reconstruction_h

#include <vector>
#include <set>
#include <map>
#include <stack>
//#include "Composite.h"
#include "CBUtils.h"
#include "tree_matching/swc_utils.h"

class Reconstruction;
class BranchContainer;
class Connection;
class DecisionPoint;
class Composite;

class CompositeBranchContainer;

struct TreeSegment : public NeuronSegment{
    TreeSegment * parent;
    vector<TreeSegment*> child_list;
    vector<MyMarker*> markers;
};
NeuronSegment * convert_to_neuron_segment(TreeSegment * root);
TreeSegment * convert_to_tree_segment(NeuronSegment * root);
void delete_tree(TreeSegment * root, bool delete_markers);
void delete_neuron(NeuronSegment * root, bool delete_markers);

/**
 * Contains the confidence value of the branch, a pointer to the branch data, and pointers to either end's deicision point
 */
class BranchContainer {
protected:
    Reconstruction * bc_reconstruction;
    NeuronSegment * bc_segment;
//    TreeSegment * bc_segment;
    BranchContainer * bc_parent;
    std::set<BranchContainer *> bc_children;
    CompositeBranchContainer * bc_composite_match;
    double bc_confidence;
    /*
    Connection * parent_connection;
    std::set<Connection *> child_connections;
*/
public:
    // Contructors
    BranchContainer();
    BranchContainer(Reconstruction * reconstruction);
    BranchContainer(Reconstruction * reconstruction, NeuronSegment * segment, BranchContainer * parent=nullptr, CompositeBranchContainer * composite_match=nullptr, double confidence=-1);
    ~BranchContainer();
    
    // Setters
    void set_segment(NeuronSegment * segment);
    void set_reconstruction(Reconstruction * reconstruction);
    void set_parent(BranchContainer * parent);
    void add_child(BranchContainer * branch);
    void add_children(std::set<BranchContainer *> branches);
    void remove_children();
    void remove_child(BranchContainer * child);
    //void set_children(std::set<BranchContainer *> children);
    void set_composite_match(CompositeBranchContainer * composite_match);
    void set_confidence(double confidence);
    
    BranchContainer * split_branch();
    BranchContainer * split_branch(std::size_t const split_point);
    
    // Getters
    NeuronSegment * get_segment() const;
    BranchContainer * get_parent() const;
    std::set<BranchContainer *> get_children() const;
    CompositeBranchContainer * get_composite_match() const;
    double get_confidence() const;
    Connection * get_parent_connection() const;
    std::set<Connection *> get_child_connection() const;
    Reconstruction * get_reconstruction() const;
    
    bool operator < (const BranchContainer& branch) const
    {
        return (bc_confidence < branch.bc_confidence);
    }
};

class branch_ptr_less{
public:
    bool operator()(const BranchContainer * lhs, const BranchContainer * rhs) const{
        return !lhs ||
        (rhs &&
         (lhs->get_confidence() < rhs->get_confidence() ||
          (lhs->get_confidence() == rhs->get_confidence() &&
           segment_ptr_less()(lhs->get_segment(), rhs->get_segment()))));
    }
};

class Reconstruction {
    string r_name;
    NeuronSegment * r_tree;
    SegmentPtrSet r_segments;
    std::map<NeuronSegment *,BranchContainer *> segment_container_map;
    BranchContainer * r_root; // Should point to r_tree
    double r_confidence;
    
    void create_branch_container_trees(BranchContainer * root_branch);
public:
    Reconstruction();
    Reconstruction(NeuronSegment * tree);
    Reconstruction(string name, NeuronSegment * tree, double confidence=1);
    ~Reconstruction();
    //  Reconstruction(string name, BranchContainer * branchRoot, double confidence=1);
    
    void update_branch_by_segment(NeuronSegment * segment, BranchContainer * branch);
    void add_branch(BranchContainer * branch);
    void update_constituents();
    
    void set_name(string name);
    void set_tree(NeuronSegment * tree);
    void set_confidence(double confidence);
    
    string get_name() const;
    NeuronSegment * get_tree() const;
    SegmentPtrSet get_segments() const;
    std::vector<NeuronSegment *> get_segments_ordered();
    std::vector<BranchContainer *> get_branches(); // Using vector so that children always come after their parents
    BranchContainer * get_root_branch();
    BranchContainer * get_branch_by_segment(NeuronSegment * segment);
    double get_confidence() const;
    Reconstruction * copy();
};


std::vector<NeuronSegment *> produce_segment_vector(NeuronSegment * root);
SegmentPtrSet produce_segment_set(NeuronSegment * root);
std::map<NeuronSegment *, BranchContainer *> produce_segment_container_map(BranchContainer * container_root);
// Shortest distance between line and point if perpendicular is on segment, otherwise shortest distance to either end
double point_segment_distance(MyMarker s1, MyMarker s2, MyMarker p, MyMarker &closest_on_line);
// Shortest distance between two line segments if the perpendicular of the lines is on both segments, otherwise returns -1
double segments_perpendicular_distance(MyMarker l11, MyMarker l12, MyMarker l21, MyMarker l22, MyMarker &closest1, MyMarker &closest2);

NeuronSegment * copy_segment_tree(NeuronSegment * root);
NeuronSegment * copy_segment_markers(NeuronSegment* segment);

#endif
