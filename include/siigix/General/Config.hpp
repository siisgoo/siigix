#ifndef CONFIG_HPP_K1X5DCXO
#define CONFIG_HPP_K1X5DCXO

//!!!!!!!!!!!!!!!!!!!! MOVE IT TO SOURCE FILE SUKO!

#include <memory>
#include <siigix/General/eprintf.hpp>
#include <stdexcept>
#include <functional>
#include <string>
#include <map>
#include <vector>
#include <any>

/*
 * Config structrue is a multy node-leaf Tree
 *
 * ConfigUnit - is a Leaf
 * ConfigNode - is a Node
 * Config - is a Root
 *
 */

namespace sgx {

    using SingleUnit = std::any;
    using ArrayUnit = std::vector<SingleUnit>;

    enum UnitType {
        UNIT_EMPTY = -1,
        UNIT_SINGLE_TYPE,
        UNIT_ARRAY_TYPE,
    };

    class Unit {
        private:
            ArrayUnit   _load;
            std::string _name;

        public:
            std::string& name() { return _name; }
            const std::string& name() const { return _name; }
            UnitType type() const
            {
                switch (_load.size()) {
                    case 0: return UNIT_EMPTY;
                    case 1: return UNIT_SINGLE_TYPE;
                    default: return UNIT_ARRAY_TYPE;
                }
            }

            void set(const ArrayUnit& arr) {
                if (arr.size() == 0) {
                    return;
                }
                for (auto& item: arr) {
                    _load.push_back(item);
                }
            }

            const SingleUnit& get() const      { return _load[0]; }
            const ArrayUnit&  getArray() const { return _load; }

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

    class ConfigNode;  /* Node */
    using ConfigUnit  = Unit; /* Leaf */
    using ConfigUnits = std::vector<ConfigUnit>;
    /* using ConfigSubNodesVec = std::vector<ConfigNode*>; */
    using ConfigNodeVec = std::vector<std::unique_ptr<ConfigNode>>;
    using NodeIterator  = ConfigNodeVec::iterator;
    using UnitsIterator = ConfigUnits::iterator;

    // REMOVE IT
    class IConfigNode {
        protected:
            IConfigNode();

        public:
            virtual bool operator == (const ConfigNode& other) = 0;
            virtual bool operator != (const ConfigNode& other) = 0;

            virtual bool isNoSubNodes() const = 0;
            virtual bool isNoUnits() const = 0;

            virtual const std::string& name() const = 0;
            virtual void setName(const std::string&) = 0;
            virtual int subNodesCount() const = 0;
            virtual const ConfigNodeVec& getSubNodes() const = 0;
            virtual const ConfigUnits&  getUnits() const = 0;
            virtual const ConfigNode* getParent() const = 0;

            virtual UnitsIterator findUnit(std::string field, bool rc = false) = 0;
            virtual NodeIterator  findNode(std::string name, bool rec = false) = 0;

            virtual void addUnit(const ConfigUnit& new_unit) = 0;
            virtual void addUnit(const std::string& name, const SingleUnit& value) = 0;
            virtual void addUnit(const std::string& name, const ArrayUnit& array) = 0;
            virtual void removeUnit(const UnitsIterator& iter) = 0;
            virtual void removeUnit(const std::string& name) = 0;
            virtual void removeUnit(const ConfigUnit& remove_unit) = 0;

            virtual void addSubNode(const std::string& name) = 0; //add empty one
            virtual void addSubNode(std::unique_ptr<ConfigNode> other) = 0; //add exist
            virtual bool removeSubNode(const NodeIterator& iter, bool rec = false) = 0;
            virtual bool removeSubNode(std::string name, bool rec = false) = 0;

            virtual ~IConfigNode();
    };

    class ConfigNode : public IConfigNode {
        protected:
            std::string _name;
            ConfigUnits _units;
            ConfigNodeVec _sub_nodes;
            /* IConfigNode* _parent; */
            ConfigNode* _parent;
        public:

            virtual const std::string& name() const override { return _name; }
            virtual void setName(const std::string& new_name) override { _name = new_name; }
            virtual int  subNodesCount() const override { return _sub_nodes.size(); }

            virtual bool operator == (const ConfigNode& other) override {
                return this->_name == other.name();
            }

            virtual bool operator != (const ConfigNode& other) override {
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

            virtual const ConfigNodeVec& getSubNodes() const override {
                return _sub_nodes;
            }
            virtual const ConfigUnits& getUnits() const override {
                return _units;
            }
            virtual const ConfigNode* getParent() const override {
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
            virtual void addUnit(const ConfigUnit& new_unit) override              { _units.push_back(new_unit); }
            virtual void addUnit(const std::string& name, const SingleUnit& value) override { this->addUnit(ConfigUnit(name, value)); }
            virtual void addUnit(const std::string& name, const ArrayUnit& array)  override { this->addUnit(ConfigUnit(name, array)); }
            virtual void removeUnit(const UnitsIterator& iter) override            { _units.erase(iter); }
            virtual void removeUnit(const std::string& name) override {
                for (UnitsIterator it = _units.begin(); it != _units.end(); it++) {
                    if (it->name() == name) {
                        _units.erase(it);
                        break;
                    }
                }
            }
            virtual void removeUnit(const ConfigUnit& unit) override { removeUnit(unit.name()); }

            virtual void addSubNode(const std::string& name) override {
                _sub_nodes.push_back(std::make_unique<ConfigNode>(name, this));
            }

            virtual void addSubNode(std::unique_ptr<ConfigNode> other) override {
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

            ConfigNode(std::string name, ConfigNode* parent = nullptr)
                : _name(name), _parent(parent)
            { }

            virtual ~ConfigNode() { }
    };
} /* sgx  */

#endif /* end of include guard: CONFIG_HPP_K1X5DCXO */
