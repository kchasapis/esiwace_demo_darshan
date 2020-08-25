#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <string.h>
#include <getopt.h>

/**
 *   Simple function to permute randomly elements within an array
 */
void build_random_permuation(int* interval, int nb_elt)
{
    int i = 0;
    int holder, permute;

    srand((unsigned int)MPI_Wtime());
 
    for (i = 0 ; i < nb_elt ; i++)
    {
        holder = interval[i];
        permute = i + (rand() % (nb_elt -i));
        interval[i] = interval[permute];
        interval[permute] = holder;
    }
}

void sample_error(int error, char *string)
{
  fprintf(stderr, "Error %d in %s\n", error, string);
  MPI_Finalize();
  exit(-1);
}

int main (int argc, char **argv)
{
    char *workspace = NULL;
    int block_size = 0;
    int index;
    int c;
    int verbose = 0;
    double start = 0, end = 0;
    int rank, size;
    int result;

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
        block_size = atoi(optarg) * 1024;
        break;
      case'm':     //block_size in MiB
        block_size = atoi(optarg) * 1024 * 1024;
        break;
      case '?':
        if (optopt =='p' || optopt == 'l' || optopt =='k' || optopt == 'm' || optopt =='v'){
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
        fprintf(stderr, "Non-option argument %s\n", argv[index]);
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
     printf ("Each rank writes a block of size %d bytes to a single shared file\n", block_size);
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

    for (int k = 0; k < block_size; k++)
    {
        sprintf(fill, "%d", rankm);
        fill+=1 ;
    }
    fill-=block_size;

   /* Generate an array of offsets for 1K random write access to the file
    */
    int *offset_range;
    offset_range = (int*) malloc(sizeof(int) * block_size / 1024);
    if(offset_range == NULL)
    {
    fprintf(stderr, "ERROR: unable to allocate the offset table\n");
    exit(EXIT_FAILURE);
    }

    for (int i = 0; i < block_size / 1024 ; i++)
       offset_range[i] = i; 
    build_random_permuation(offset_range, block_size / 1024);

  /* Each rank writes between offsets rank*block_size and (rank+1)*block_size
   * that is to say writes a block of size block_size 
   */

  printf ("[rank %2d] Opening file %s\n", rank, path);
  int error = MPI_File_open(MPI_COMM_WORLD, path, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    if (error != MPI_SUCCESS)
    {
        fprintf(stderr,"ERROR: unable to open file %s\n", path);
        MPI_Abort(MPI_COMM_WORLD, 911);
    }


    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0)
    {
        start = MPI_Wtime();
    }

    /* offset is the base offset for the current process
     * plus the index in the permutation array (expressed in KB)
     */
    for(int i = 0 ; i < block_size / 1024; i++)
    {
        //MPI_Offset disp = rank*block_size + offset_range[i] * 1024;
        MPI_Offset disp = rank*block_size + (i * 1024);
        result=MPI_File_write_at(fh, disp , fill, 1024, MPI_CHAR, MPI_STATUS_IGNORE);
        if(result != MPI_SUCCESS)
             sample_error(result, "MPI_File_write_at");
    }
    MPI_Barrier(MPI_COMM_WORLD); //All ranks synchronize

    if (rank == 0)      //rank 0 measures time after 2nd barrier
    {
        end = MPI_Wtime();
        double time = (double) (end - start);
        double bw = (double)size*block_size/(time*1024*1024);
        if (verbose && rank ==0)
        {
            printf("The I/O write operations took %f seconds\n", time);
            printf("Bandwidth: %.1f MB/s\n", bw);
            printf("csv output: # processes, block size, total write time\n");
        }
        printf("\t%d, %d, \t\t%f\n",size, block_size, time);
    }

    free(fill);
    free(path);
    MPI_File_close(&fh);
    MPI_Finalize();
    return EXIT_SUCCESS;
    }

