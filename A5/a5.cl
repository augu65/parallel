__kernel void a5(__global float* data,  __global float* given) {

   int local_addr;
   local_addr = get_local_id(0);
   uint size = given[1];
   int begin = (size*local_addr)/given[0];
   int end = (size*(local_addr+1))/given[0];
   //loops for number of rows
   for(int i=1; i < size; i++){
      //loops for the number of columns
      for (int j=begin; j < end; j++ ){
         //if at left wall
         if(j == 0){
            if(data[size*(i-1)+j] != -1 && data[size*(i-1)+j+1] != -1){
               data[(size*i) + j] = local_addr;
            }else if(data[size*(i-1)+j] != -1 && data[size*(i-1)+j+1] == -1){
               data[(size*i) + j] = local_addr;
            }if(data[size*(i-1)+j] == -1 && data[size*(i-1)+j+1] != -1){
               data[(size*i) + j] = local_addr;
            }
         // if at right wall
         }else if(j+1 - size == 0){
            if(data[size*(i-1)+j] != -1 && data[size*(i-1)+j-1] == -1){
               data[(size*i) + j] = local_addr;
            }else if(data[size*(i-1)+j-1] != -1 && data[size*(i-1)+j] == -1){
               data[(size*i) + j] = local_addr;
            }
         // if not at wall
         }else{
            if(data[size*(i-1)+j] != -1 && data[size*(i-1)+j-1] == -1 && data[size*(i-1)+j+1] == -1){
               data[(size*i) + j] = local_addr;
            }else if(data[size*(i-1)+j] != -1 && data[size*(i-1)+j-1] == -1 && data[size*(i-1)+j+1] != -1){
               data[(size*i) + j] = local_addr;
            }else if(data[size*(i-1)+j] == -1 && data[size*(i-1)+j-1] == -1 && data[size*(i-1)+j+1] != -1){
               data[(size*i) + j] = local_addr;
            }else if(data[size*(i-1)+j] == -1 && data[size*(i-1)+j-1] != -1 && data[size*(i-1)+j+1] == -1){
               data[(size*i) + j] = local_addr;
            }
         }
      }
      barrier(CLK_LOCAL_MEM_FENCE);
   }
}
