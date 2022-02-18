#ifndef CONFIG_HPP_K1X5DCXO
#define CONFIG_HPP_K1X5DCXO

//!!!!!!!!!!!!!!!!!!!! MOVE IT TO SOURCE FILE SUKO!
#include "siigix/General/eprintf.hpp"

#include <memory>
#include <stdexcept>
#include <functional>
#include <string>
#include <map>
#include <vector>
#include <any>

/*
 * Markup structrue is a multy node-leaf Tree
 *
 * MarkupUnit - is a Leaf
 * MarkupNode - is a Node
 * Markup - is a Root
 *
 */

namespace sgx::Markup {

    using SingleUnit = std::any;
    using ArrayUnit = std::vector<SingleUnit>;

    enum UnitType {
        UNIT_EMPTY = -1,
        UNIT_SINGLE_TYPE,
        UNIT_ARRAY_TYPE,
    };

    //add iterator operations
    class Unit {
        private:
            ArrayUnit   _load;
            std::string _name;

        public:
            std::string& name() { return _name; }
            const std::string& name() const { return _name; }

            bool has_value() const { return _load[0].has_value(); }

            void clear() { _load.clear(); }

            void setAny(const std::any& v){ clear(); _load.push_back(v); }
            void setInt(const int v)      { clear(); _load.push_back(v); }
            void setBool(const int v)     { clear(); _load.push_back(v); }
            void setDouble(const int v)   { clear(); _load.push_back(v); }
            void setString(const int v)   { clear(); _load.push_back(v); }
            void setArray(const ArrayUnit& v) { clear(); _load = v; }
            void append(const std::any& v){ _load.push_back(v); }

            std::any& operator[] (int i)        { return _load[i]; }
            const std::any& getAny() const      { return _load[0]; }
            const int getInt() const            { return std::any_cast<int>(_load[0]); }
            const int getBool() const           { return std::any_cast<bool>(_load[0]); }
            const int getDouble() const         { return std::any_cast<double>(_load[0]); }
            const std::string getString() const { return std::any_cast<std::string>(_load[0]); }
            const ArrayUnit getArray() const    { return _load; }

            //null if its array
            const std::type_info& type(const int i) const { return _load[i].type(); }

            Unit()
            { }

            Unit(const std::string& name, const SingleUnit& u)
                : _load({u}),
                _name(name)
            { }

            Unit(const std::string& name, const ArrayUnit& au)
                : _load(au),
                _name(name)
            { }

            Unit(const Unit& other)
                : _load(other._load),
                _name(other._name)
            {  }
    };

    class MarkupNode;  /* Node */
    using MarkupUnit  = Unit; /* Leaf */
    using MarkupUnits = std::vector<MarkupUnit>;
    /* using MarkupSubNodesVec = std::vector<MarkupNode*>; */
    using MarkupNodeVec = std::vector<std::unique_ptr<MarkupNode>>;
    using NodeIterator  = MarkupNodeVec::iterator;
    using UnitsIterator = MarkupUnits::iterator;

    // REMOVE IT
    class IMarkupNode {
        protected:
            IMarkupNode();

        public:
            virtual bool operator == (const MarkupNode& other) = 0;
            virtual bool operator != (const MarkupNode& other) = 0;

            virtual MarkupNode* operator[] (const std::string& node_name) = 0;

            virtual bool isNoSubNodes() const = 0;
            virtual bool isNoUnits() const = 0;

            virtual const std::string& name() const = 0;
            virtual void setName(const std::string&) = 0;
            virtual int subNodesCount() const = 0;
            virtual const MarkupNodeVec& getSubNodes() const = 0;
            virtual const MarkupUnits&  getUnits() const = 0;
            virtual const MarkupNode* getParent() const = 0;

            virtual UnitsIterator findUnit(std::string field, bool rc = false) = 0;
            virtual NodeIterator  findNode(std::string name, bool rec = false) = 0;

            virtual void addUnit(const MarkupUnit& new_unit) = 0;
            virtual void addUnit(const std::string& name, const SingleUnit& value) = 0;
            virtual void addUnit(const std::string& name, const ArrayUnit& array) = 0;
            virtual void removeUnit(const UnitsIterator& iter) = 0;
            virtual void removeUnit(const std::string& name) = 0;
            virtual void removeUnit(const MarkupUnit& remove_unit) = 0;

            virtual void addSubNode(const std::string& name) = 0; //add empty one
            virtual void addSubNode(std::unique_ptr<MarkupNode> other) = 0; //add exist
            virtual bool removeSubNode(const NodeIterator& iter, bool rec = false) = 0;
            virtual bool removeSubNode(std::string name, bool rec = false) = 0;

            virtual ~IMarkupNode();
    };

    //rewrite search algorithms
    class MarkupNode : public IMarkupNode {
        protected:
            std::string _name;
            MarkupUnits _units;
            MarkupNodeVec _sub_nodes;
            /* IMarkupNode* _parent; */
            MarkupNode* _parent;
        public:
            virtual MarkupNode* operator[] (const std::string& node_name) override
            {
                return findNode(node_name)->get();
            }

            virtual const std::string& name() const override { return _name; }
            virtual void setName(const std::string& new_name) override { _name = new_name; }
            virtual int  subNodesCount() const override { return _sub_nodes.size(); }

            virtual bool operator == (const MarkupNode& other) override {
                return this->_name == other.name();
            }

            virtual bool operator != (const MarkupNode& other) override {
                return !(*this == other);
            }

            virtual bool isNoSubNodes() const override {
                if (_sub_nodes.size() == 0) {
                    return true;
                }
                return false;
            }

            virtual bool isNoUnits() const override {
                if (_units.size() == 0) {
                    return true;
                }
                return false;
            }

            virtual const MarkupNodeVec& getSubNodes() const override {
                return _sub_nodes;
            }
            virtual const MarkupUnits& getUnits() const override {
                return _units;
            }
            virtual const MarkupNode* getParent() const override {
                return _parent;
            }

            virtual UnitsIterator findUnit(std::string name, bool rec = false) override {
                auto it = _units.begin();
                for (auto& u: _units) {
                    if (name == u.name()) {
                        return it;
                    }
                    it++;
                }

                if (rec) {
                    for (auto& n: _sub_nodes) { //its ok? TEST TODO
                        auto attempt = n->findUnit(name, true);
                        if (attempt != _units.end()) {
                            return attempt;
                        }
                    }
                }

                return _units.end();
            }

            virtual NodeIterator findNode(std::string name, bool rec = false) override {
                auto it = _sub_nodes.begin();
                for (auto& node: _sub_nodes) {
                    if (node->_name == name) {
                        return it;
                    }
                    it++;
                }

                if (rec) {
                    for (auto& node: _sub_nodes) {
                        NodeIterator attempt = node->findNode(name, true);
                        if (attempt != _sub_nodes.end()) {
                            return attempt;
                        }
                    }
                }

                return _sub_nodes.end();
            }

            /* do net check for existence */
            virtual void addUnit(const MarkupUnit& new_unit) override              { _units.push_back(new_unit); }
            virtual void addUnit(const std::string& name, const SingleUnit& value) override { this->addUnit(MarkupUnit(name, value)); }
            virtual void addUnit(const std::string& name, const ArrayUnit& array)  override { this->addUnit(MarkupUnit(name, array)); }
            virtual void removeUnit(const UnitsIterator& iter) override            { _units.erase(iter); }
            virtual void removeUnit(const std::string& name) override {
                for (UnitsIterator it = _units.begin(); it != _units.end(); it++) {
                    if (it->name() == name) {
                        _units.erase(it);
                        break;
                    }
                }
            }
            virtual void removeUnit(const MarkupUnit& unit) override { removeUnit(unit.name()); }

            virtual void addSubNode(const std::string& name) override {
                _sub_nodes.push_back(std::make_unique<MarkupNode>(name, this));
            }

            virtual void addSubNode(std::unique_ptr<MarkupNode> other) override {
                other->_parent = this;
                _sub_nodes.push_back(std::move(other));
            }

            virtual bool removeSubNode(const NodeIterator& iter, bool rec = false) override {
                bool deleted = false;
                for (auto& node: _sub_nodes) {
                    if (*node == **iter) {
                        _sub_nodes.erase(iter);
                        deleted = true;
                    }
                }

                if (rec) {
                    for (auto& node: _sub_nodes) {
                        if (node->removeSubNode(iter, true)) {
                            return true;
                        }
                    }
                }

                return deleted;
            }

            virtual bool removeSubNode(std::string name, bool rec = false) override {
                NodeIterator nodeIter = findNode(name, rec);
                if (nodeIter != _sub_nodes.end()) {
                    _sub_nodes.erase(nodeIter);
                    return true;
                }

                return false;
            }

            MarkupNode(std::string name, MarkupNode* parent = nullptr)
                : _name(name), _parent(parent)
            { }

            virtual ~MarkupNode() { }
    };
} /* sgx  */

#endif /* end of include guard: CONFIG_HPP_K1X5DCXO */
