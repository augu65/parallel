#define PROGRAM_FILE "a5.cl"
#define KERNEL_FUNC "a5"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// provide version of OpenCL to use
#define CL_TARGET_OPENCL_VERSION 220

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device()
{

  cl_platform_id platform;
  cl_device_id dev;
  int err;

  /* Identify a platform */
  err = clGetPlatformIDs(1, &platform, NULL);
  if (err < 0)
  {
    perror("Couldn't identify a platform");
    exit(1);
  }

  /* Access a device */
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
  if (err == CL_DEVICE_NOT_FOUND)
  {
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
  }
  if (err < 0)
  {
    perror("Couldn't access any devices");
    exit(1);
  }

  return dev;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
{

  cl_program program;
  FILE *program_handle;
  char *program_buffer, *program_log;
  size_t program_size, log_size;
  int err;

  /* Read program file and place content into buffer */
  program_handle = fopen(filename, "r");
  if (program_handle == NULL)
  {
    perror("Couldn't find the program file");
    exit(1);
  }
  fseek(program_handle, 0, SEEK_END);
  program_size = ftell(program_handle);
  rewind(program_handle);
  program_buffer = (char *)malloc(program_size + 1);
  program_buffer[program_size] = '\0';
  fread(program_buffer, sizeof(char), program_size, program_handle);
  fclose(program_handle);

  /* Create program from file */
  program = clCreateProgramWithSource(ctx, 1,
                                      (const char **)&program_buffer, &program_size, &err);
  if (err < 0)
  {
    perror("Couldn't create the program");
    exit(1);
  }
  free(program_buffer);

  /* Build program */
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err < 0)
  {

    /* Find size of log and print to std output */
    clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                          0, NULL, &log_size);
    program_log = (char *)malloc(log_size + 1);
    program_log[log_size] = '\0';
    clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                          log_size + 1, program_log, NULL);
    printf("%s\n", program_log);
    free(program_log);
    exit(1);
  }

  return program;
}

int main( int argc, char ** argv)
{

  /* OpenCL structures */
  cl_device_id device;
  cl_context context;
  cl_program program;
  cl_kernel kernel;
  cl_command_queue queue;
  cl_int err;
  size_t local_size, global_size;

  int kernals = 1;
  int size = 20;
  int config = 0;
  //get user input
  if (argc > 1) {
    int argPtr = 1;
    while(argPtr < argc) {
      if (strcmp(argv[argPtr], "-n") == 0) {
        sscanf(argv[argPtr+1], "%d", &kernals);
        argPtr += 2;
      } else if (strcmp(argv[argPtr], "-s") == 0) {
        sscanf(argv[argPtr+1], "%d", &size);
        argPtr += 2;
        if(size < 7){
          printf("please enter a size 7 or larger.\ngoodbye.\n");
          return 0;
        }
      }else if (strcmp(argv[argPtr], "-i") == 0) {
        sscanf(argv[argPtr+1], "%d", &config);
        argPtr += 2;
        if(config > 4){
          config = 0;
          printf("pattern does not exist. Implementing random\n");
        }
      }
    }
  }
    global_size = kernals;
    local_size = kernals;

  /* Data and buffers */
  float given[3];
  given[0] = kernals;
  given[1] = size;
  given[2] = config;
  float data[size * size];
  time_t t;
  cl_mem input_buffer, given_buffer;
  //initialize data array to -1
  for (int i=0; i < size * size; i++){
    data[i] = -1;
  }
  //setup starting pattern
  //seed randomness
  if (config == 0){
    srand((unsigned) time(&t));
    for (int i=0; i< size; i++){
      if (rand()%2 == 1){
        data[i] = 0;
      }
    }
  }else if (config == 1) {
    data[(size/2)-2] = 0;
    data[(size/2)] = 0;
    data[(size/2)+1] = 0;
  } else if (config == 2) {
    data[(size/2)-3] = 0;
    data[(size/2)-2] = 0;
    data[(size/2)-1] = 0;
    data[(size/2)] = 0;
    data[(size/2)+1] = 0;
    data[(size/2)+2] = 0;
  } else if (config == 3) {
    data[(size/2)-2] = 0;
    data[(size/2)] = 0;
    data[(size/2)+1] = 0;
    data[(size/2)+2] = 0;
  } else if (config == 4) {
    data[(size/2)-3] = 0;
    data[(size/2)-2] = 0;
    data[(size/2)-1] = 0;
    data[(size/2)] = 0;
    data[(size/2)+1] = 0;
    data[(size/2)+2] = 0;
    data[(size/2)+3] = 0;
  }

  /* Create device and context */
  device = create_device();
  context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
  if (err < 0)
  {
    perror("Couldn't create a context");
    exit(1);
  }

  /* Build program */
  program = build_program(context, device, PROGRAM_FILE);

  /* Create data buffer */
  input_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, (size * size) * sizeof(float), data, &err);
  //pass flag data
  given_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3 * sizeof(float), given, &err);
  if (err < 0)
  {
    perror("Couldn't create a buffer");
    exit(1);
  };

  /* Create a command queue */
  //   clCreateCommandQueue depricated, use clCreateCommandQueueWithProperties();
  //   original code: queue = clCreateCommandQueue(context, device, 0, &err);
  queue = clCreateCommandQueueWithProperties(context, device, 0, &err);
  if (err < 0)
  {
    perror("Couldn't create a command queue");
    exit(1);
  };

  /* Create a kernel */
  kernel = clCreateKernel(program, KERNEL_FUNC, &err);
  if (err < 0)
  {
    perror("Couldn't create a kernel");
    exit(1);
  };

  /* Create kernel arguments */
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &given_buffer);
  if (err < 0)
  {
    perror("Couldn't create a kernel argument");
    exit(1);
  }

  /* Enqueue kernel */
  err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size,
                               &local_size, 0, NULL, NULL);
  if (err < 0)
  {
    perror("Couldn't enqueue the kernel");
    exit(1);
  }

  /* Read the kernel's output */
  err = clEnqueueReadBuffer(queue, input_buffer, CL_TRUE, 0,
                            sizeof(data), data, 0, NULL, NULL);
  if (err < 0)
  {
    perror("Couldn't read the buffer");
    exit(1);
  }
  //print out array
  for (int i = 0; i < size * size; i++){
    //-1 means its a space
    if(data[i] != -1){
      printf("%0.f", data[i]);
    }else{
      printf(" ");
    }
    if((i +1) % size == 0){
      printf("\n");
    }
  }

  /* Deallocate resources */
  clReleaseKernel(kernel);
  clReleaseMemObject(input_buffer);
  clReleaseMemObject(given_buffer);
  clReleaseCommandQueue(queue);
  clReleaseProgram(program);
  clReleaseContext(context);
  return 0;
}
