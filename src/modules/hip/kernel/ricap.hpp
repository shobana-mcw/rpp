#include <hip/hip_runtime.h>
#include "hip/rpp_hip_common.hpp"

template <typename T>
__global__ void ricap_pkd_tensor(T *srcPtr,
                                 uint2 srcStridesNH,
                                 T *dstPtr,
                                 uint2 dstStridesNH,
                                 uint *permutedIndices,
                                 RpptROIPtr crop_region,
                                 RpptROIPtr roiTensorPtrSrc)
{
    int id_x = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x) * 8;
    int id_y = hipBlockIdx_y * hipBlockDim_y + hipThreadIdx_y;
    int id_z = hipBlockIdx_z * hipBlockDim_z + hipThreadIdx_z;

    uint srcIdx, dstIdx, permuteIdx;
    d_float8 pix_f8;

    if ((id_y >= roiTensorPtrSrc[id_z].xywhROI.roiHeight) || (id_x >= roiTensorPtrSrc[id_z].xywhROI.roiWidth * 3))
    {
        return;
    }

    permuteIdx = id_z * 4;

    if ((id_x >= 0) && (id_y >= 0) && (id_y <= crop_region[0].xywhROI.roiHeight) && (id_x <= crop_region[0].xywhROI.roiWidth * 3))
        srcIdx = (permutedIndices[permuteIdx] * srcStridesNH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNH.y) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x * 3);
    else if ((id_y >= 0) && (id_x >= crop_region[0].xywhROI.roiWidth * 3) && (id_y <= (crop_region[1].xywhROI.roiHeight)) && (id_x <= (roiTensorPtrSrc[id_z].xywhROI.roiWidth * 3)))
        srcIdx = (permutedIndices[permuteIdx + 1] * srcStridesNH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNH.y) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x * 3);
    else if ((id_y >= crop_region[1].xywhROI.roiHeight) && (id_x >= 0) && (id_y <= (roiTensorPtrSrc[id_z].xywhROI.roiHeight)) && (id_x <= crop_region[2].xywhROI.roiWidth * 3))
        srcIdx = (permutedIndices[permuteIdx + 2] * srcStridesNH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNH.y) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x * 3);
    else if ((id_y >= crop_region[1].xywhROI.roiHeight) && (id_x >= crop_region[2].xywhROI.roiWidth * 3) && (id_y <= (roiTensorPtrSrc[id_z].xywhROI.roiHeight)) && (id_x <= (roiTensorPtrSrc[id_z].xywhROI.roiWidth * 3)))
        srcIdx = (permutedIndices[permuteIdx + 3] * srcStridesNH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNH.y) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x * 3);

    dstIdx = (id_z * dstStridesNH.x) + (id_y * dstStridesNH.y) + id_x;

    rpp_hip_load8_and_unpack_to_float8(srcPtr, srcIdx, &pix_f8);
    rpp_hip_pack_float8_and_store8(dstPtr, dstIdx, &pix_f8);
}

template <typename T>
__global__ void ricap_pln_tensor(T *srcPtr,
                                 uint3 srcStridesNCH,
                                 T *dstPtr,
                                 uint3 dstStridesNCH,
                                 int channelsDst,
                                 uint *permutedIndices,
                                 RpptROIPtr crop_region,
                                 RpptROIPtr roiTensorPtrSrc)
{

    int id_x = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x) * 8;
    int id_y = hipBlockIdx_y * hipBlockDim_y + hipThreadIdx_y;
    int id_z = hipBlockIdx_z * hipBlockDim_z + hipThreadIdx_z;

    uint srcIdx, dstIdx, permuteIdx;
    d_float8 pix_f8;

    if ((id_y >= roiTensorPtrSrc[id_z].xywhROI.roiHeight) || (id_x >= roiTensorPtrSrc[id_z].xywhROI.roiWidth))
    {
        return;
    }

    permuteIdx = id_z * 4;

    if ((id_x >= 0) && (id_y >= 0) && (id_y <= crop_region[0].xywhROI.roiHeight) && (id_x <= crop_region[0].xywhROI.roiWidth))
        srcIdx = (permutedIndices[permuteIdx] * srcStridesNCH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNCH.z) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x);
    else if ((id_y >= 0) && (id_x >= crop_region[0].xywhROI.roiWidth) && (id_y <= (crop_region[1].xywhROI.roiHeight)) && (id_x <= (roiTensorPtrSrc[id_z].xywhROI.roiWidth)))
        srcIdx = (permutedIndices[permuteIdx + 1] * srcStridesNCH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNCH.z) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x);
    else if ((id_y >= crop_region[1].xywhROI.roiHeight) && (id_x >= 0) && (id_y <= (roiTensorPtrSrc[id_z].xywhROI.roiHeight)) && (id_x <= crop_region[2].xywhROI.roiWidth))
        srcIdx = (permutedIndices[permuteIdx + 2] * srcStridesNCH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNCH.z) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x);
    else if ((id_y >= crop_region[1].xywhROI.roiHeight) && (id_x >= crop_region[2].xywhROI.roiWidth) && (id_y <= (roiTensorPtrSrc[id_z].xywhROI.roiHeight)) && (id_x <= (roiTensorPtrSrc[id_z].xywhROI.roiWidth)))
        srcIdx = (permutedIndices[permuteIdx + 3] * srcStridesNCH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNCH.z) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x);

    dstIdx = (id_z * dstStridesNCH.x) + (id_y * dstStridesNCH.z) + id_x;

    rpp_hip_load8_and_unpack_to_float8(srcPtr, srcIdx, &pix_f8);
    rpp_hip_pack_float8_and_store8(dstPtr, dstIdx, &pix_f8);

    if (channelsDst == 3)
    {
        srcIdx += srcStridesNCH.y;
        dstIdx += dstStridesNCH.y;

        rpp_hip_load8_and_unpack_to_float8(srcPtr, srcIdx, &pix_f8);
        rpp_hip_pack_float8_and_store8(dstPtr, dstIdx, &pix_f8);

        srcIdx += srcStridesNCH.y;
        dstIdx += dstStridesNCH.y;

        rpp_hip_load8_and_unpack_to_float8(srcPtr, srcIdx, &pix_f8);
        rpp_hip_pack_float8_and_store8(dstPtr, dstIdx, &pix_f8);
    }
}

template <typename T>
__global__ void ricap_pkd3_pln3_tensor(T *srcPtr,
                                       uint2 srcStridesNH,
                                       T *dstPtr,
                                       uint3 dstStridesNCH,
                                       uint *permutedIndices,
                                       RpptROIPtr crop_region,
                                       RpptROIPtr roiTensorPtrSrc)
{
    int id_x = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x) * 8;
    int id_y = hipBlockIdx_y * hipBlockDim_y + hipThreadIdx_y;
    int id_z = hipBlockIdx_z * hipBlockDim_z + hipThreadIdx_z;

    if ((id_y >= roiTensorPtrSrc[id_z].xywhROI.roiHeight) || (id_x >= roiTensorPtrSrc[id_z].xywhROI.roiWidth))
    {
        return;
    }

    uint srcIdx, dstIdx, permuteIdx;

    d_float24 pix_f24;

    permuteIdx = id_z * 4;

    if ((id_x >= 0) && (id_y >= 0) && (id_y <= crop_region[0].xywhROI.roiHeight) && (id_x <= crop_region[0].xywhROI.roiWidth))
        srcIdx = (permutedIndices[permuteIdx] * srcStridesNH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNH.y) + ((id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x) * 3);
    else if ((id_y >= 0) && (id_x >= crop_region[0].xywhROI.roiWidth) && (id_y <= (crop_region[1].xywhROI.roiHeight)) && (id_x <= (roiTensorPtrSrc[id_z].xywhROI.roiWidth)))
        srcIdx = (permutedIndices[permuteIdx + 1] * srcStridesNH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNH.y) + ((id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x) * 3);
    else if ((id_y >= crop_region[1].xywhROI.roiHeight) && (id_x >= 0) && (id_y <= (roiTensorPtrSrc[id_z].xywhROI.roiHeight)) && (id_x <= crop_region[2].xywhROI.roiWidth))
        srcIdx = (permutedIndices[permuteIdx + 2] * srcStridesNH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNH.y) + ((id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x) * 3);
    else if ((id_y >= crop_region[1].xywhROI.roiHeight) && (id_x >= crop_region[2].xywhROI.roiWidth) && (id_y <= (roiTensorPtrSrc[id_z].xywhROI.roiHeight)) && (id_x <= (roiTensorPtrSrc[id_z].xywhROI.roiWidth)))
        srcIdx = (permutedIndices[permuteIdx + 3] * srcStridesNH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNH.y) + ((id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x) * 3);

    dstIdx = (id_z * dstStridesNCH.x) + (id_y * dstStridesNCH.z) + id_x;

    rpp_hip_load24_pkd3_and_unpack_to_float24_pln3(srcPtr, srcIdx, &pix_f24);
    rpp_hip_pack_float24_pln3_and_store24_pln3(dstPtr, dstIdx, dstStridesNCH.y, &pix_f24);
}

template <typename T>
__global__ void ricap_pln3_pkd3_tensor(T *srcPtr,
                                       uint3 srcStridesNCH,
                                       T *dstPtr,
                                       uint2 dstStridesNH,
                                       uint *permutedIndices,
                                       RpptROIPtr crop_region,
                                       RpptROIPtr roiTensorPtrSrc)
{
    int id_x = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x) * 8;
    int id_y = hipBlockIdx_y * hipBlockDim_y + hipThreadIdx_y;
    int id_z = hipBlockIdx_z * hipBlockDim_z + hipThreadIdx_z;

    if ((id_y >= roiTensorPtrSrc[id_z].xywhROI.roiHeight) || (id_x >= roiTensorPtrSrc[id_z].xywhROI.roiWidth))
    {
        return;
    }

    uint srcIdx, dstIdx, permuteIdx;
    d_float24 pix_f24;

    permuteIdx = id_z * 4;

    if ((id_x >= 0) && (id_y >= 0) && (id_y <= crop_region[0].xywhROI.roiHeight) && (id_x <= crop_region[0].xywhROI.roiWidth))
        srcIdx = (permutedIndices[permuteIdx] * srcStridesNCH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNCH.z) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x);
    else if ((id_y >= 0) && (id_x >= crop_region[0].xywhROI.roiWidth) && (id_y <= (crop_region[1].xywhROI.roiHeight)) && (id_x <= (roiTensorPtrSrc[id_z].xywhROI.roiWidth)))
        srcIdx = (permutedIndices[permuteIdx + 1] * srcStridesNCH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNCH.z) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x);
    else if ((id_y >= crop_region[1].xywhROI.roiHeight) && (id_x >= 0) && (id_y <= (roiTensorPtrSrc[id_z].xywhROI.roiHeight)) && (id_x <= crop_region[2].xywhROI.roiWidth))
        srcIdx = (permutedIndices[permuteIdx + 2] * srcStridesNCH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNCH.z) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x);
    else if ((id_y >= crop_region[1].xywhROI.roiHeight) && (id_x >= crop_region[2].xywhROI.roiWidth) && (id_y <= (roiTensorPtrSrc[id_z].xywhROI.roiHeight)) && (id_x <= (roiTensorPtrSrc[id_z].xywhROI.roiWidth)))
        srcIdx = (permutedIndices[permuteIdx + 3] * srcStridesNCH.x) + ((id_y + roiTensorPtrSrc[id_z].xywhROI.xy.y) * srcStridesNCH.z) + (id_x + roiTensorPtrSrc[id_z].xywhROI.xy.x);

    dstIdx = (id_z * dstStridesNH.x) + (id_y * dstStridesNH.y) + id_x * 3;

    rpp_hip_load24_pln3_and_unpack_to_float24_pkd3(srcPtr, srcIdx, srcStridesNCH.y, &pix_f24);
    rpp_hip_pack_float24_pkd3_and_store24_pkd3(dstPtr, dstIdx, &pix_f24);
}

template <typename T>
RppStatus hip_exec_ricap_tensor(T *srcPtr,
                                RpptDescPtr srcDescPtr,
                                T *dstPtr,
                                RpptDescPtr dstDescPtr,
                                Rpp32u *permutationTensor,
                                RpptROIPtr roiTensorPtrSrc,
                                RpptROIPtr cropRegion,
                                rpp::Handle &handle)
{
    int localThreads_x = 16;
    int localThreads_y = 16;
    int localThreads_z = 1;
    int globalThreads_x = (dstDescPtr->strides.hStride + 7) >> 3;
    int globalThreads_y = dstDescPtr->h;
    int globalThreads_z = handle.GetBatchSize();

    if ((srcDescPtr->layout == RpptLayout::NHWC) && (dstDescPtr->layout == RpptLayout::NHWC))
    {
        hipLaunchKernelGGL(ricap_pkd_tensor,
                           dim3(ceil((float)globalThreads_x / localThreads_x), ceil((float)globalThreads_y / localThreads_y), ceil((float)globalThreads_z / localThreads_z)),
                           dim3(localThreads_x, localThreads_y, localThreads_z),
                           0,
                           handle.GetStream(),
                           srcPtr,
                           make_uint2(srcDescPtr->strides.nStride, srcDescPtr->strides.hStride),
                           dstPtr,
                           make_uint2(dstDescPtr->strides.nStride, dstDescPtr->strides.hStride),
                           permutationTensor,
                           cropRegion,
                           roiTensorPtrSrc);
    }
    else if ((srcDescPtr->layout == RpptLayout::NCHW) && (dstDescPtr->layout == RpptLayout::NCHW))
    {
        hipLaunchKernelGGL(ricap_pln_tensor,
                           dim3(ceil((float)globalThreads_x / localThreads_x), ceil((float)globalThreads_y / localThreads_y), ceil((float)globalThreads_z / localThreads_z)),
                           dim3(localThreads_x, localThreads_y, localThreads_z),
                           0,
                           handle.GetStream(),
                           srcPtr,
                           make_uint3(srcDescPtr->strides.nStride, srcDescPtr->strides.cStride, srcDescPtr->strides.hStride),
                           dstPtr,
                           make_uint3(dstDescPtr->strides.nStride, dstDescPtr->strides.cStride, dstDescPtr->strides.hStride),
                           dstDescPtr->c,
                           permutationTensor,
                           cropRegion,
                           roiTensorPtrSrc);
    }
    else if ((srcDescPtr->c == 3) && (dstDescPtr->c == 3))
    {
        if ((srcDescPtr->layout == RpptLayout::NHWC) && (dstDescPtr->layout == RpptLayout::NCHW))
        {
            hipLaunchKernelGGL(ricap_pkd3_pln3_tensor,
                               dim3(ceil((float)globalThreads_x / localThreads_x), ceil((float)globalThreads_y / localThreads_y), ceil((float)globalThreads_z / localThreads_z)),
                               dim3(localThreads_x, localThreads_y, localThreads_z),
                               0,
                               handle.GetStream(),
                               srcPtr,
                               make_uint2(srcDescPtr->strides.nStride, srcDescPtr->strides.hStride),
                               dstPtr,
                               make_uint3(dstDescPtr->strides.nStride, dstDescPtr->strides.cStride, dstDescPtr->strides.hStride),
                               permutationTensor,
                               cropRegion,
                               roiTensorPtrSrc);
        }
        else if ((srcDescPtr->layout == RpptLayout::NCHW) && (dstDescPtr->layout == RpptLayout::NHWC))
        {
            globalThreads_x = (srcDescPtr->strides.hStride + 7) >> 3;
            hipLaunchKernelGGL(ricap_pln3_pkd3_tensor,
                               dim3(ceil((float)globalThreads_x / localThreads_x), ceil((float)globalThreads_y / localThreads_y), ceil((float)globalThreads_z / localThreads_z)),
                               dim3(localThreads_x, localThreads_y, localThreads_z),
                               0,
                               handle.GetStream(),
                               srcPtr,
                               make_uint3(srcDescPtr->strides.nStride, srcDescPtr->strides.cStride, srcDescPtr->strides.hStride),
                               dstPtr,
                               make_uint2(dstDescPtr->strides.nStride, dstDescPtr->strides.hStride),
                               permutationTensor,
                               cropRegion,
                               roiTensorPtrSrc);
        }
    }

    return RPP_SUCCESS;
}
