__kernel void sum ( __global const unsigned char* input,
                    __global long *partialSums,
                    __local  int *localSums)
{
    /*printf("HELLO");
    uint local_id = get_local_id(0);
    uint group_size = get_local_size(0);

    localSums[local_id] = (long)input[get_global_id(0)];

    for (uint stride = group_size/2; stride>0; stride /=2)
    {
        barrier(CLK_LOCAL_MEM_FENCE);
        if (local_id < stride)
            localSums[local_id] += localSums[local_id + stride];
    }
    if (local_id == 0)
    partialSums[get_group_id(0)] = localSums[0];*/
    partialSums[get_group_id(0)] = 255;

}                                     