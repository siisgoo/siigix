#ifndef CONFIG_HPP_K1X5DCXO
#define CONFIG_HPP_K1X5DCXO

//!!!!!!!!!!!!!!!!!!!! MOVE IT TO SOURCE FILE SUKO!

#include <siigix/General/eprintf.hpp>
#include <string>
#include <map>
#include <vector>

/*
 * Config structrue is a multy node-leaf Tree
 *
 * ConfigUnit - is a Leaf
 * ConfigNode - is a Node
 * Config - is a Root
 *
 */

namespace sgx {

    /* enum UnitType { */
    /*     integer, */
    /*     real, */
    /*     string, */
    /*     boolean, */
    /* }; */

    /* struct Unit { */
    /*     union { */
    /*         int integer; */
    /*         double real; */
    /*         std::string string; */
    /*         bool boolean; */
    /*     }; */
    /*     int type; */
    /* }; */

    class IconfigNode;
    class ConfigRoot; /* Root */
    class ConfigNode; /* Node */
    /* first - field name; second - value */
    using ConfigUnit = std::pair<std::string, std::string>; /* Leaf */
    using ConfigUnits = std::map<std::string, std::string>; /* Leaf map */
    bool cmpConfigNode(const ConfigNode& a, const ConfigNode& b);


    using NodeIterator  = std::vector<ConfigNode*>::iterator;
    using UnitsIterator = ConfigUnits::iterator;

    class IConfigNode {
        protected:
            IConfigNode() {  }

        public:
            virtual std::string name() const = 0;
            virtual int subNodesCount() const = 0;

            virtual UnitsIterator findUnit(std::string field, bool rc = false) = 0;
            virtual NodeIterator  findNode(std::string name, bool rec = false) = 0;

            virtual void addUnit(const ConfigUnit& new_unit) = 0;
            virtual void addUnit(const std::string& field, const std::string& value) = 0;
            virtual void removeUnit(const UnitsIterator& iter) = 0;
            virtual void removeUnit(const std::string& key) = 0;
            virtual void removeUnit(const ConfigUnit& remove_unit) = 0;

            virtual ConfigNode* addSubNode(std::string name) = 0;
            virtual bool removeSubNode(const NodeIterator& iter, bool rec = false) = 0;
            virtual bool removeSubNode(std::string name, bool rec = false) = 0;

            virtual ~IConfigNode();
    };

    class ConfigNode : public IConfigNode {
        protected:
            std::string _name;
            ConfigUnits _units;
            ConfigNode* _parent;
            std::vector<ConfigNode*> _sub_nodes;

        public:

            /*
             * find value by field(key) name
             * return non nullptr as sign of success
             * can find recursivly, pass rec as true for this purpose
             */
            virtual UnitsIterator findUnit(std::string field, bool rec = false) {
                auto it = _units.begin();
                for (auto& u: _units) {
                    if (field == u.first) {
                        return it;
                    }
                    it++;
                }

                if (rec) {
                    for (auto& n: _sub_nodes) { //its ok? TEST TODO
                        auto attempt = n->findUnit(field, true);
                        if (attempt != _units.end()) {
                            return attempt;
                        }
                    }
                }

                return _units.end();
            }

            virtual NodeIterator findNode(std::string name, bool rec = false) {
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

            virtual std::string name() const  { return _name; };
            virtual int subNodesCount() const { return _sub_nodes.size(); }

            /* do net check for existence */
            virtual void addUnit(const ConfigUnit& new_unit)                         { _units[new_unit.first] = new_unit.second; }
            virtual void addUnit(const std::string& field, const std::string& value) { _units[field] = value; }
            virtual void removeUnit(const UnitsIterator& iter)                       { _units.erase(iter); }
            virtual void removeUnit(const std::string& key)                          { _units.erase(key); }
            virtual void removeUnit(const ConfigUnit& remove_unit)                   { _units.erase(remove_unit.first); }

            virtual ConfigNode* addSubNode(std::string name) {
                ConfigNode * node(new ConfigNode(name, this));
                _sub_nodes.push_back(node);
                return node;
            }

            virtual bool removeSubNode(const NodeIterator& iter, bool rec = false) {
                bool deleted = false;
                for (auto& node: _sub_nodes) {
                    if (cmpConfigNode(*node, **iter)) {
                        _sub_nodes.erase(iter);
                        delete node;
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

            virtual bool removeSubNode(std::string name, bool rec = false) {
                NodeIterator nodeIter = findNode(name, rec);
                if (nodeIter != _sub_nodes.end()) {
                    _sub_nodes.erase(nodeIter);
                    delete &nodeIter; /* ??? */
                    return true;
                }

                return false;
            }

            ConfigNode(ConfigNode* parent = nullptr)
                : ConfigNode("ConfigNode", parent)
            { }

            ConfigNode(std::string name, ConfigNode* parent = nullptr)
                : _name(name), _parent(parent)
            { }

            virtual ~ConfigNode() {
                delete _parent;
                for (auto node: _sub_nodes) {
                    delete node;
                }
            }
    };

    inline bool cmpConfigNode(const ConfigNode& a, const ConfigNode& b) {
        return a.name() == b.name();
    }

    struct ConfigRoot : public ConfigNode {
        public:
            ConfigRoot() : ConfigNode("Root") { }
    };

} /* sgx  */ 

#endif /* end of include guard: CONFIG_HPP_K1X5DCXO */
