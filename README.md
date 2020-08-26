        DDN IO Profiling Lab

  We assume the availability of a MPI (mpich) distribution on your system

  What is it?
  -----------

The DDN IO Profiling lab is this set of files and directories that
together will allow a student to work through some examples to improve
their understanding of IO Profiling methods.

  How do I use it?
  ----------------

Starting from dir1, read the README file in that directory, follow the
instructions and complete the exercise. Then move to dir2, etc



dir 1 -> importance of  metadata: single shared file versus file per process
dir 2 -> importance of pattern: writing the same amount of data with random or sequential access
dir 3 -> detect in an application the sub-optimal part
dir 4 -> playing with MPI collective operations


Most of the examples can be run with simple shell script. We encourage you
to edit these script to play a bit with some parameters:

- play with Darshan monitoring option
- increase decrease the amount of data written by the codes


Remember that file systems are very good at caching and can deliver surprisingly high performances
as long as the data volume is kept below a given threshold.

  Which tool should I use?
  ----------------


While many IO tools are available on the market we've focused this session to Darshan.
Do not take this as a Darshan training but more as a way to understand I/O decision impact

- all benchmarks have been instrumented at source level to
record the time spent in MPI-IO. MPI-Timer are set around
the critical I/O part of the code, and the total bandwidth
is reported


  Darshan
  ----------------
Darshan is a convenient tool for I/O profiling, using LD_PRELOAD
is dynamically override the mpi/posix I/O function and build comprehensive
report at th end of the application
Darshsan is versatile, Darshan is cool, Darshan is system-wide
but Darsahn is not managing time stamp
For stability reason we recommend to use at the moment Darshan 3.1.1

= To install locally Darshan runtime

cd darshan-$version/darshan-runtime/

./prepare

./configure ./configure CC=mpicc --prefix=$installation-dir
--with-log-path-by-env=DARSHAN_LOGPATH --with-jobid-env=NONE --with-mem-align=128

make -j 32 && make install

This will compile and generate the Darshan library that captures the I/O. You can find the library at the install directory that was specified with the prefix or at the default location if the prefix was not specified. Also, under the lib folder of this directory.

The DARSHAN_LOGPATH enviroment variable should be set to specify the directory that Darshan will store the log file.

= To install locally Darshan util

cd darshan-$version/darshan-util/

./prepare

./configure CC=mpicc --prefix=$installation-dir

make -j 32 && make install

This will compile and generate the Darshan utils that you will need for the post process of the log files. You can find the utils at the install directory that was specified with the prefix or at the default location if the prefix was not specified. Also, under the bin folder of this directory.

Extra packages:

libbz2 development headers and library (libbz2-dev or similar)
Perl
pdflatex
gnuplot 4.2 or later
epstopdf

For Ubuntu:
sudo apt-get install texlive-latex-extra texlive-font-utils libpod-latex-perl gnuplot

-------------------------

Run the examples in the directories (dir1, dir2, etc.)

Set DARSHAN_LIB enviroment variable to point to the Darshan library

export DARSHAN_LIB=$lib_instalation_path

Once you have spot your own log file, you can ask Darshan to generate a pdf file that will contain a summary:

darshan-job-summary.pl $log_file

---------------------

Darshan DXT

For more detailed views you can enable Darshan DXT module. This module is already integrated in the source code. To activate it you have to set the DXT_ENABLE_IO_TRACE environment variable.

export DXT_ENABLE_IO_TRACE=1

For the DXT analysis you can use darshan-dxt-parser tool.


