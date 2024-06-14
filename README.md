# Set up development & test environments
This describes the setup used by us for development and testing. For development it was primarily don on windows with visual studio 2022, the guide can be found below **Setup for windows**. We primarily used the LSF 10 cluster for testing and running experiments. The guide for how we accessed and cloned the repository can be found in **Setup linux HPC (LSF 10)** and a guide on how to compile the clone repository can be found in **Compile for linux to run on HPC cluster**.

## Setup for windows
For the setup on windows, make sure you have visual studio installed (we used visual studio 2022) and the c++ development package (can be found in visual studio installer). To set up MPI follow the guides below where you first need to download two files for MPI setup (run them both and follow the install wizard). Then there is a simple configuration of how to set up the project (no need to do that again as it is done for the project already) but you can do it yourself and test it out to see that it works. **Note you might need to restart your computer before running the program** 
* Get downloads: https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi
* Get started: https://learn.microsoft.com/en-us/archive/blogs/windowshpc/how-to-compile-and-run-a-simple-ms-mpi-program (Try rebooting before running)

## Compile for linux to run on HPC cluster
See **Setup linux HPC (LSF 10)** for how to get access to the HPC cluster we used. 
1. To compile we first need to get access to a dynamicalle allocated node by running
   ```sh
   linuxsh
   ```
2. A few modules needs to be loaded
   ```sh
   module load gcc/12.2.0-binutils-2.39
   module load mpi/4.1.4-gcc-12.2.0-binutils-2.39
   module load make/4.4.1
   module load module-git 
   ```
3. Then clone the repository if not done already
   ```sh
   git clone https://<PAT>@github.com/JacobHej/EDA_cpp.git
   ```
4. Navigate to (whatever you want to compile MPIHelloWorld will be replaced)
5. Run 
   ```sh
   make
   ```
   And you should have the file to execute using   
   ```sh
   mpiexec -n <number of cores> ./output
   ```
   To run for batch job do make and then
   ```sh
   bsub < scripts/MPI_job_config.sh
   ```
   To check status  
   ```sh
   bstat
   ```
6. Zip Results
   Work from EDA_cpp folder
   ```sh
   zip -r Results.zip Results
   ```

* makefile doc: https://makefiletutorial.com/
  
## Setup linux HPC (LSF 10)
To use the HPC cluster first get the vpn app AnyConnect (Cisco), then download the thinlinc client to access the cluster and finally generate a github PAT to clone the repository through the commandline. See links below for each guide.
* Get vpn: https://www.inside.dtu.dk/en/medarbejder/it-og-telefoni/it-support-og-kontakt/guides/remote/vpn-cisco-anyconnect
* Get thinlinc: https://www.gbar.dtu.dk/linkcollection/thinlinc.html
* Get token for github: https://kettan007.medium.com/how-to-clone-a-git-repository-using-personal-access-token-a-step-by-step-guide-ab7b54d4ef83

### Culster docs
Here are some general documentation on the HPC cluster we used.
* General: https://www.hpc.dtu.dk/?page_id=2513
* Modules: https://www.hpc.dtu.dk/?page_id=282
* Batch jobs: https://www.hpc.dtu.dk/?page_id=1416#Defaults

# Performance improvements
## random numbers
![image](https://github.com/JacobHej/EDA_cpp/assets/38204838/bd8b44bb-bb7d-4d68-95b2-10cf50f119d9)

time: 52.8237ms before update
time: 16.7624ms after optimize

next_v1: 0.0194ms
next_v2: 0.1582ms
next_v3: 0.0196ms
```cpp
//https://thompsonsed.co.uk/random-number-generators-for-c-performance-tested
uint64_t shuffle_table[4];
uint64_t next_v1(void)
{
    uint64_t s1 = shuffle_table[0];
    uint64_t s0 = shuffle_table[1];
    uint64_t result = s0 + s1;
    shuffle_table[0] = s0;
    s1 ^= s1 << 23;
    shuffle_table[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    return result % 100;
}

int next_v2() {
    return rand() % 100;
}

//https://en.wikipedia.org/wiki/Xorshift
struct xorshift128p_state {
    uint64_t x[2];
};

/* The state must be seeded so that it is not all zero */
int next_v3(struct xorshift128p_state* state)
{
    uint64_t t = state->x[0];
    uint64_t const s = state->x[1];
    state->x[0] = s;
    t ^= t << 23;		// a
    t ^= t >> 18;		// b -- Again, the shifts and the multipliers are tunable
    t ^= s ^ (s >> 5);	// c
    state->x[1] = t;
    return (t + s) % 100;
}
```

```cpp
   Timer timer;
    volatile int x;
    shuffle_table[0] = rand();
    shuffle_table[1] = rand();
    shuffle_table[2] = rand();
    shuffle_table[3] = rand();
    timer.start();
    for (int i = 0; i < 10000; i++) {
        x = next_v1();
    }
    timer.stop();
    std::cout << "next_v1: " << timer.measure() / 1000000 << "ms" << std::endl;

    timer.start();
    for (int i = 0; i < 10000; i++) {
        x = next_v2();
    }
    timer.stop();
    std::cout << "next_v2: " << timer.measure() / 1000000 << "ms" << std::endl;

    xorshift128p_state state;
    state.x[1] = rand();
    state.x[0] = rand();
    timer.start();
    for (int i = 0; i < 10000; i++) {
        x = next_v3(&state);
    }
    timer.stop();
    std::cout << "next_v3: " << timer.measure() / 1000000 << "ms" << std::endl;
```

## vector index [] instead of .at
time: 12.658ms
```cpp
samples[j][k] = next_v3(&state) < dist[k] ? 1 : 0;
samples.at(j).at(k) = next_v3(&state) < dist[k] ? 1 : 0;
```
