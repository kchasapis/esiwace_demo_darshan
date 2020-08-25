# ESiWACE Summer School 2020 - Darshan demo
All the following steps assume you are running on the [ESiWACE Summer School VM](https://docs.google.com/document/d/1f9dDjTSnaCtGoJNQYa8eb3U6jCjQ_rjmVsDSo16cRzk)

## Installing dependencies
```bash
sudo apt-get install bzip2 texlive-latex-extra texlive-font-utils libpod-latex-perl gnuplot
```

## Compiling Darshan
```bash
git clone https://github.com/villegar/esiwace_demo_darshan.git
tar -xzvf SUMMER_SCHOOL_2020.tar.gz
cd SUMMER_SCHOOL_2020
tar -xzvf darshan-3.2.1.tar.gz
cd darshan-3.2.1
cd darshan-runtime/
./configure CC=mpicc --with-log-path-by-env=DARSHAN_LOGPATH --with-mem-align=128 --with-jobid-env=NONE
sudo make -j install
cd ..
cd darshan-util/
./configure CC=mpicc
sudo make -j install
```

## Running the examples
### Example 1 (`SUMMER_SCHOOL_2020/dir1`)
```bash
cd ../../dir1 # Assuming you were inside SUMMER_SCHOOL_2020/darshan-3.2.1/darshan-util
export DARSHAN_LOGPATH=./
export DARSHAN_LIB=/usr/local/lib/libdarshan.so
./singleshared.sh -c 1 -r 4 
# You might want to change the 4 by 2, based on the resources allocated to your VM
darshan-parser *.darshan
for f in $(ls *.darshan) ; do
  echo "Processing -> $f"
  darshan-job-summary.pl $f
done
```

### Example 2 (`SUMMER_SCHOOL_2020/dir2`)
```bash
cd ../dir2 # Assuming you were inside SUMMER_SCHOOL_2020/dir1 from the previous example
export DXT_ENABLE_IO_TRACE=1
```

### Example 3 (`SUMMER_SCHOOL_2020/dir3`)
```bash
cd ../dir3 # Assuming you were inside SUMMER_SCHOOL_2020/dir2 from the previous example
# TODO
```

### Example 4 (`SUMMER_SCHOOL_2020/dir4`)
```bash
cd ../dir4 # Assuming you were inside SUMMER_SCHOOL_2020/dir3 from the previous example
# TODO
```
