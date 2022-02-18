#include "siigix/Data/Markup/MarkupSignatures.hpp"

#include <stdexcept>

namespace sgx::Markup {

    signatureManager * signatureManager::_smInstance = nullptr;

    signatureManager::signatureManager() {
        if (_smInstance == nullptr) {
            _smInstance = this;
        } else {
            throw std::runtime_error(eprintf("Zachm singleton dwajdi vizivaem????"));
        }
    }

    signatureManager::~signatureManager()
    {  }

    const ISignature *
    signatureManager::getSignature(const std::string& signName) {
        for (auto& storedSign: _smInstance->_signatures) {
            if (storedSign.first == signName) {
                return storedSign.second;
            }
        }
        throw std::runtime_error(eprintf("signatureManager::", __func__, " No suck signature: ", signName));
    }

    void
    signatureManager::addSignature(const std::string& name, ISignature * sign) {
        _smInstance->_signatures[name] = sign;
    }

} /* sgx  */ 
