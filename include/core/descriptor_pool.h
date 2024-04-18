//#ifndef REND_DESCRIPTOR_POOL_H
//#define REND_DESCRIPTOR_POOL_H
//
//#include "core/descriptor_set.h"
//#include "core/rend_defs.h"
//
//namespace rend
//{
//
//class DescriptorPool
//{
//public:
//    explicit DescriptorPool(const DescriptorPoolInfo& info);
//    ~DescriptorPool(void);
//    DescriptorPool(const DescriptorPool&)            = delete;
//    DescriptorPool(DescriptorPool&&)                 = delete;
//    DescriptorPool& operator=(const DescriptorPool&) = delete;
//    DescriptorPool& operator=(DescriptorPool&&)      = delete;
//
//    DescriptorPoolHandle handle(void) const;
//
//    DescriptorSetHandle allocate_descriptor_set(const DescriptorSetInfo& info);
//    void                dealloacte_descriptor_set(DescriptorSetHandle handle);
//    DescriptorSet*      get_descriptor_set(DescriptorSetHandle handle) const;
//
//private:
//    DescriptorPoolHandle _handle{ NULL_HANDLE };
//
//    DataArray<DescriptorSet> _descriptor_sets;
//};
//
//}
//#endif
