# Installation and Usage

@ Author: **Shitao Weng** (*sweng@andrew.cmu.edu*)
@ Author: **Shushan Chen** (*shushanc@andrew.cmu.edu*)

## Install

Our project run well on `MacOS 10.9.3`, `Ubuntu 14.04 LTS` and `Ubuntu 13.10`

Here we give a install process of our project in `Ubuntu 13.10`. This process is similar as in other `*nix` system.

### Install libgtk2.0.dev

*If you alreadly have libgtk2, pls skip this section*

```bash
$ sudo apt-get install libgtk2.0-dev
```

### Install OpenCV 2.4.9

*If you alreadly have opencv 2.4.9 installed in your computer, great! you can skip this section*
Test
#### Download

Please download opencv-2.4.9 from this link:  http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.4.9/opencv-2.4.9.zip/download

#### Compile and install

**IMPORTANT: Before you install opencv, pls confirm that you have libgtk-2.0-dev already**

```bash
$ unzip opencv-2.4.9.zip
$ mkdir opencv-build
$ cd opencv-build
$ cmake ../opencv-2.4.9
$ make 
$ sudo make install

$ sudo ldconfig
```

### Install our project

```bash
$ unzip group7.zip
$ cd group7
$ mkdir build
$ cmake ../src
$ make
```

After doing these it should generate several demo excutable file and data files in build/ directory:

* datas:
    * `img/` : Contain all the test image files
    * `haarcascades/`: Face detection trained data from opencv.
* demos
    * `kd_demo`
    * `match2img`
    * `match2db`
    * `accuracyTest`
    * `camera`

*Following give a guide of how to run these demos*.

### DEMO 
*This section give a introduction of how to run the demo program, for details about how they work, pls refer to our code manual*

#### DEMO 1. kd_demo

This is actually not a good demo program, it is created to test if the bbf search in our implementation of KD-TREE is good or not.

##### Execution 
```bash
pls do it in build/

./kd_demo
```

##### Excepted Output
```
...
Naive n^2 search: 251.400080
kdTree bbf-search: 112.205169 from [0][./img/demo/job_inbook_50.png]
Naive n^2 search: 112.205169
kdTree bbf-search: 116.589022 from [0][./img/demo/job_inbook_50.png]
Naive n^2 search: 116.589022
kdTree bbf-search: 173.499280 from [0][./img/demo/job_inbook_50.png]
Naive n^2 search: 173.499280
kdTree bbf-search: 167.508209 from [0][./img/demo/job_inbook_50.png]
Naive n^2 search: 167.508209
...
```

As you know the nearest point search in **BBF KD-Tree** randomly drop some search point that seems to generate very bad result. This is a tradeoff between time and accurary when searching a nearest neighbor in a big feature database.

And if you run this demo, you can see our bbf search acts pretty well.

##### Parameters
For this demo, you can change the input image and template image, you can also change the threshold(simply the ceil euclidean distance between two 128-leow ngth features):

```bash
Usage: ./kd_demo -i input file name -t template file name -b threshold

e.g.:

./kd_demo -i img/demo/job_inbook_50.png -t img/demo/job_inbook.png -b 150
```

#### DEMO 2. match2img

This is a demo to show how a image match to a template image.

##### Execution 
```bash
pls do it in build/

./match2img
```

##### Excepted Output
![Alt text](./1417264295417.png)

##### Parameters
For this demo, you can change the input image and template image, you can also change the threshold(simply the ceil euclidean distance between two 128-length features):

```bash
Usage: ./match2img -i input file name -t template file name -b threshold

e.g.:

./match2img -i img/demo/job_inbook_50.png -t img/demo/job_inbook.png -b 130
```

**Note: It is important to adjust the threshold when you try to change the input files. When we search in a big database rather than a single image, we will use another more effective way to set a ratio between the best and the second best value rather than a threshold**


#### DEMO 3. match2db

This is a demo to show how a image match to a big database.

##### Execution 
```bash
pls do it in build/

./match2db
```

##### Excepted Output
![Alt text](./1417264539517.png)

It simply draws the detected name on the face. You can get the name from the filename. Here we input file `./img/att/test/s9_8.pgm` and get the result `s9`(Come from the train datas!!) on his face.

##### Parameters
For this demo, you can change the input image and template directory, you can also change the ratio (simply the ceil ratio between the best and second best match, $[0.4,0.9]$) and the back track times on the search of kd-tree. Lower backTrackRatio means higher accuracy and slower performance. Its reasonable value is $[2, 10]$.

```bash
Usage: ./match2db -i input file name -t template directory -b ratio -k brackTrackRatio
e.g.:

./match2db -t img/att/train -i ./img/att/test/s10_10.pgm -b 0.8 -k 2
```

#### DEMO 4. accuracyTest

This is a demo to test the match accuracy, it input a training image directory and a test image directory. Using the image in training directory to build a big `KD-TREE`, and reconition every image in the test directory using `bbf-search`.

##### Execution 
```bash
pls do it in build/

./accuracyTest
```

##### Excepted Output

```bash
Test accuracy: 1 : 1 : 200
Test accuracy: 2 : 2 : 200
...
```

It is `correct_count:tested_count:size_of_test_database`

*This process will take half an hour testing on 200files-to-200files match*

##### Parameters

For this demo, you can change the input image and template directory, you can also change the ratio (simply the ceil ratio between the best and second best match, $[0.4,0.9]$) and the back track times on the search of kd-tree. Lower backTrackRatio means higher accuracy and slower performance. Its reasonable value is $[2, 10]$.

```bash
Usage: ./match2db -i input directory -t template directory -b ratio -k backTrackRatio
e.g.:

./accuracyTest -t img/att/train -i ./img/att/test/ -b 0.8 -k 2
```
By default, it will test on `att` database. If you want to test on `yaleface` database:


```bash
./accuracyTest -t img/yaleface/train -i img/yaleface/test
```

We will give a report of accuracy in the presentation. 

#### DEMO 5. camera

This is a demo to detect and recognition a face in real-time vedio camera.

##### Execution 
```bash
pls do it in build/

./camera
```

##### Excepted Output
![Alt text](./1417266520500.png)

##### Parameter 

For this demo, you can change the template directory, you can also change the ratio (simply the ceil ratio between the best and second best match, $[0.4,0.9]$) and the back track times on the search of kd-tree. Lower backTrackRatio means higher accuracy and slower performance. Its reasonable value is $[2, 10]$.

```bash
Usage: ./camera -t template directory -b ratio -k backTrackRatio
e.g.:

./camera -t img/ -b 0.8 -k 2
```
