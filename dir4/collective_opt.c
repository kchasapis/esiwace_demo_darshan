#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <string.h>
#include <getopt.h>

int main (int argc, char **argv){
  char *workspace = NULL;
  int block_size = 0;
  int index;
  int c;
  int verbose = 0;
  double start = 0, end = 0;
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_File fh;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  opterr = 0;
  while ((c = getopt (argc, argv, "vp:k:m:")) != -1)
    switch (c)
      {
      case 'v':    //verbose mode
        verbose = 1;
        break;
      case 'p':    //path were files are written
        workspace = optarg;
        break;
      case 'k':    //block_size in KiB
        block_size = (int)strtol(optarg, (char **)NULL, 10)*1024;
        break;
      case 'm':     //block_size in MiB
        block_size = (int)strtol(optarg, (char **)NULL, 10)*1024*1024;
        break;
      case '?':
        if (optopt =='p' || optopt =='k' || optopt == 'm' || optopt =='v'){
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        }
        else if (isprint (optopt)){
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        }
        else{
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        exit(EXIT_FAILURE);
      default:
        abort ();
      }

  if (block_size == 0)
  {
    fprintf(stderr,"Argument block_size is not set\nUse options -k (KiB) or - m (MiB)\n");
    exit(EXIT_FAILURE);
  }

  if(size == 1)
  {
    fprintf(stderr,"Number of processes should be more than one \nTry setting np > 1 with mpirun\n");
    exit(EXIT_FAILURE);
  }

  for (index = optind; index < argc; index++)
  {
    fprintf (stderr, "Non-option argument %s\n", argv[index]);
    exit(EXIT_FAILURE);
  }

  if(verbose && rank ==0)
  {
     if(workspace == NULL)
     {
     printf("Output file to be written as ./big.data. To specify an alternate output file uses -p option\n");
     }
     else
     {
        printf("Output file to be written as %s\n", workspace);
     }
     printf ("Each rank writes a block of size %.2f MiB to the shared file\n", (double)block_size/(1024*1024));
  }
  /* Created files are /path/small-{rank} */
  char *path = (char *) malloc((strlen(workspace) + 6 + 4 +1));
  if(path == NULL)
  {
    fprintf (stderr, "Rank %d unable to allocate memory\n", rank);
    exit(EXIT_FAILURE);
  }
  sprintf(path, "%s", workspace);

  char *fill = (char *) malloc((block_size+1) *sizeof(char));
  if(fill == NULL)
  {
    fprintf (stderr, "Rank %d unable to allocate memory\n", rank);
    exit(EXIT_FAILURE);
  }
  int rankm=rank%10;

  int k;
  for (k=0; k<block_size;k++)
  {
    sprintf(fill, "%d", rankm);
    fill+=1 ;
  }
  fill-=block_size;


  /* Each rank writes between offsets rank*block_size and (rank+1)*block_size
  *  that is to say writes one block block_size */
    MPI_Info info;
    MPI_Info_create(&info);
    MPI_Info_set (info, "romio_ds_write", "enable");
    MPI_Info_set (info, "romio_cb_write", "enable");
  printf ("[rank %2d] Opening file %s\n", rank, path);
  int error = MPI_File_open(MPI_COMM_WORLD, path, MPI_MODE_CREATE | MPI_MODE_WRONLY, info, &fh);
  if (error != MPI_SUCCESS)
  {
        fprintf(stderr,"ERROR: unable to open file %s\n", path);
        MPI_Abort(MPI_COMM_WORLD, 911);
  }

  MPI_Offset disp = rank*block_size;
  error = MPI_File_set_view(fh, disp, MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);

  MPI_Barrier(MPI_COMM_WORLD);
  if(rank==0)
    start = MPI_Wtime();

  MPI_File_write_all(fh, fill, block_size, MPI_CHAR, MPI_STATUS_IGNORE);
  
  MPI_File_close(&fh);
  free(fill);


  MPI_Barrier(MPI_COMM_WORLD); //All ranks synchronize
  if(rank==0)      //rank 0 measures time after 2nd barrier
  {
    end = MPI_Wtime();

    double time = (double) (end - start);
    double bw = (double)size*block_size/(time*1024*1024);
    if(verbose && rank ==0)
    {
        printf("The I/O write operations took %f seconds\n", time);
        printf("Bandwidth: %.1f MiB/s\n", bw);
        printf("Raw csv output: # processes, block size, total write time\n");
    }
    printf("%d,\t %d,\t %f\n",size, block_size, time);
  }


 // MPI_File_delete(path,MPI_INFO_NULL);
 free(path);
 MPI_Finalize();
 //return EXIT_SUCCESS;
 }

