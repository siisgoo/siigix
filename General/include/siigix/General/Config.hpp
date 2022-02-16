#ifndef CONFIG_HPP_K1X5DCXO
#define CONFIG_HPP_K1X5DCXO

//!!!!!!!!!!!!!!!!!!!! MOVE IT TO SOURCE FILE SUKO!

#include <siigix/General/eprintf.hpp>
#include <stdexcept>
#include <functional>
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

    enum SingleUnitType {
        SU_INVALID_TYPE = -1,
        SU_INTEGER_TYPE = 0,
        SU_REAL_TYPE,
        SU_STRING_TYPE,
        SU_BOOLEAN_TYPE,
    };

    union SingleUnitLoad { //mb save integer, boolean and real in double?
                           //or save all in char*/std::string and convert on getSingleUnit()?
                           //Test speed
        int         SU_INTEGER;
        double      SU_REAL;
        std::string SU_STRING;
        bool        SU_BOOLEAN;
    };

    class SingleUnit {
        private:
            SingleUnitType _type = SU_INVALID_TYPE;
            SingleUnitLoad _load;
        public:
            void setType(SingleUnitType type) { _type = type; }
            SingleUnitType type() const       { return _type; }
            const SingleUnitLoad& get() const { return _load; }
            void set(const SingleUnitLoad& load) {
                switch (_type) {
                    case SU_INVALID_TYPE: throw std::runtime_error(eprintf("SingleUnit::", __func__, " Type of SingleUnit not setted!"));

                    case SU_INTEGER_TYPE: _load.SU_INTEGER = load.SU_INTEGER;
                        break;
                    case SU_REAL_TYPE:    _load.SU_REAL = load.SU_REAL;
                        break;
                    case SU_STRING_TYPE:  _load.SU_STRING = load.SU_STRING;
                        break;
                    case SU_BOOLEAN_TYPE: _load.SU_BOOLEAN = load.SU_BOOLEAN;
                        break;
                    default:
                        throw std::domain_error(eprintf("AHTUNG! Undefined _type of SingleUnit setted: ", _type));
                }
            }
    };

    using ArrayUnit = std::vector<SingleUnit>;

    enum UnitType {
        U_INVALID_TYPE = -1,
        U_SINGLE_TYPE,
        U_ARRAY_TYPE,
    };

    class Unit {
        private:
            UnitType    _type = U_INVALID_TYPE;
            SingleUnit  _single;
            ArrayUnit   _array;
            std::string _name;

            void appendArray(const ArrayUnit& arr, int from = 0) {
                for (auto& item: arr) {
                    _array[from].set(item.get());
                }
            }
        public:
            const std::string& name() const { return _name; }
            void     setName(const std::string& new_name) { _name = new_name; }
            UnitType type() const  { return _type; }
            void     setType(const UnitType type) { _type = type; }

            void set(const ArrayUnit& arr) {
                if (arr.size() == 0) { return; }

                switch (_type) {
                    case U_INVALID_TYPE: throw std::domain_error(eprintf("Unit::", __func__, " Invalid _type of Unit setted: ", _type));
                    case U_ARRAY_TYPE:
                    {

                    }
                    case U_SINGLE_TYPE:
                    {
                        if (arr.size() != 1) {
                            throw std::runtime_error(eprintf("Unit::", __func__, " Cannot set ArrayUnit to SingleUnit"));
                        }
                        _single.setType(arr[0].type());
                        _single.set(arr[0].get());
                    }
                    default:
                        throw std::runtime_error(eprintf("Unit::", __func__, " Undefined Unit type setted: ", _type));
                };
            }
    };

    class IConfigNode; /* Interface */
    class ConfigRoot;  /* Root */
    class ConfigNode;  /* Node */
    using ConfigUnit  = Unit; /* Leaf */
    using ConfigUnits = std::vector<ConfigUnit>;
    using ConfigSubNodesVec = std::vector<ConfigNode*>; //TODO change to unique_ptr
                                                        //remove all std::vector<ConfigNode*> to ConfigSubNodesVec

    using NodeIterator  = std::vector<ConfigNode*>::iterator;
    using UnitsIterator = ConfigUnits::iterator;

    class IConfigNode {
        protected:
            IConfigNode() {  }

        public:
            virtual bool operator == (const IConfigNode& other);
            virtual bool operator != (const IConfigNode& other);

            virtual bool isNoSubNodes() const = 0;
            virtual bool isNoUnits() const = 0;

            virtual const std::string& name() const = 0;
            virtual void setName(const std::string&) = 0;
            virtual int subNodesCount() const = 0;
            virtual const std::vector<ConfigNode*>& getSubNodes() const = 0;
            virtual const ConfigUnits&  getUnits() const = 0;
            virtual const IConfigNode* getParent() const = 0;

            virtual UnitsIterator findUnit(std::string field, bool rc = false) = 0;
            virtual NodeIterator  findNode(std::string name, bool rec = false) = 0;

            virtual void addUnit(const ConfigUnit& new_unit) = 0;
            /* virtual void addUnit(const std::string& field, const std::string& value) = 0; */
            virtual void removeUnit(const UnitsIterator& iter) = 0;
            virtual void removeUnit(const std::string& name) = 0;
            virtual void removeUnit(const ConfigUnit& remove_unit) = 0;

            virtual ConfigNode* addSubNode(std::string name) = 0; //add empty one
            virtual ConfigNode* addSubNode(ConfigNode* other) = 0; //add exist
            virtual bool removeSubNode(const NodeIterator& iter, bool rec = false) = 0;
            virtual bool removeSubNode(std::string name, bool rec = false) = 0;

            virtual ~IConfigNode();
    };

    class ConfigNode : public IConfigNode {
        protected:
            std::string _name;
            ConfigUnits _units;
            IConfigNode* _parent;
            ConfigSubNodesVec _sub_nodes;
        public:

            virtual bool operator == (const IConfigNode& other) override {
                return this->_name == other.name();
            }

            virtual bool operator != (const IConfigNode& other) override {
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

            virtual const std::vector<ConfigNode*>& getSubNodes() const override {
                return _sub_nodes;
            }
            virtual const ConfigUnits& getUnits() const override {
                return _units;
            }
            virtual const IConfigNode* getParent() const override {
                return _parent;
            }

            /*
             * find value by field(key) name
             * return non nullptr as sign of success
             * can find recursivly, pass rec as true for this purpose
             */
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

            virtual const std::string& name() const override { return _name; }
            virtual void setName(const std::string& new_name) override { _name = new_name; }
            virtual int subNodesCount() const override { return _sub_nodes.size(); }

            /* do net check for existence */
            virtual void addUnit(const ConfigUnit& new_unit) override { _units.push_back(new_unit); }
            /* virtual void addUnit(const std::string& field, const std::string& value) { ; } */
            virtual void removeUnit(const UnitsIterator& iter) override { _units.erase(iter); }
            virtual void removeUnit(const std::string& name) override {
                for (UnitsIterator it = _units.begin(); it != _units.end(); it++) {
                    if (it->name() == name) {
                        _units.erase(it);
                        break;
                    }
                }
            }
            virtual void removeUnit(const ConfigUnit& remove_unit) override { removeUnit(remove_unit.name()); }

            virtual ConfigNode* addSubNode(std::string name) override {
                ConfigNode * node(new ConfigNode(name, this));
                _sub_nodes.push_back(node);
                return node;
            }

            virtual ConfigNode* addSubNode(ConfigNode* other) override {
                _sub_nodes.push_back(other);
                return other;
            }

            virtual bool removeSubNode(const NodeIterator& iter, bool rec = false) override {
                bool deleted = false;
                for (auto& node: _sub_nodes) {
                    if (*node == **iter) {
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

            virtual bool removeSubNode(std::string name, bool rec = false) override {
                NodeIterator nodeIter = findNode(name, rec);
                if (nodeIter != _sub_nodes.end()) {
                    _sub_nodes.erase(nodeIter);
                    delete &nodeIter; /* ??? */
                    return true;
                }

                return false;
            }

            ConfigNode(std::string name, IConfigNode* parent = nullptr)
                : _name(name), _parent(parent)
            { }

            virtual ~ConfigNode() {
                for (auto node: _sub_nodes) {
                    delete node;
                }
            }
    };

    struct ConfigRoot : public ConfigNode {
        public:
            ConfigRoot() : ConfigNode("Root") { }
            virtual ~ConfigRoot() { }
    };

} /* sgx  */

#endif /* end of include guard: CONFIG_HPP_K1X5DCXO */
