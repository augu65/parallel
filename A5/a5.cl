__kernel void a5(__global float* data, 
      __local float* local_result, __global float* given) {

   float sum;
   float4 input1, input2, sum_vector;
   uint global_addr, local_addr;
   uint size = given[1];
   //fix this it should start at size
   for(int i=1; i < size; i++){
      for (int j=0; j < size; j++ ){
         if(((size*i) + j) % size == 0){
            if(data[size*(i-1)+j] == 0 && data[size*(i-1)+j] == 0){
               data[(size*i) + j] = 0;
            }else if(data[size*(i-1)+j] == 0 && data[size*(i-1)+j] != 0){
               data[(size*i) + j] = 0;
            }if(data[size*(i-1)+j] != 0 && data[size*(i-1)+j] == 0){
               data[(size*i) + j] = 0;
            }
         }else if(((size*i) + j+1)%size == 0){
            if(data[size*(i-1)+j] == 0 && data[size*(i-1)+j-1] != 0){
               data[(size*i) + j] = 0;
            }else if(data[size*(i-1)+j-1] == 0 && data[size*(i-1)+j] != 0){
               data[(size*i) + j] = 0;
            }
         }else{
            if(data[size*(i-1)+j] == 0 && data[size*(i-1)+j-1] != 0 && data[size*(i-1)+j+1] != 0){
               data[(size*i) + j] = 0;
            }else if(data[size*(i-1)+j] == 0 && data[size*(i-1)+j-1] != 0 && data[size*(i-1)+j+1] == 0){
               data[(size*i) + j] = 0;
            }else if(data[size*(i-1)+j] != 0 && data[size*(i-1)+j-1] != 0 && data[size*(i-1)+j+1] == 0){
               data[(size*i) + j] = 0;
            }else if(data[size*(i-1)+j] != 0 && data[size*(i-1)+j-1] == 0 && data[size*(i-1)+j+1] != 0){
               data[(size*i) + j] = 0;
            }
         }
      }
   }

   barrier(CLK_LOCAL_MEM_FENCE);
}
