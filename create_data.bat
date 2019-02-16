@Echo off
Echo caffe create_annoset Batch
:: https://github.com/conner99/caffe/blob/ssd-microsoft/tools/convert_annoset.cpp
:: You can modify the parameters in create_data.bat if needed.
:: It will create lmdb files for trainval and test with encoded original image:
::  - G:\file\code\dnn\caffe-ssd-win-diy\data\VOC0712\trainval_lmdb
::  - G:\file\code\dnn\caffe-ssd-win-diy\data\VOC0712\test_lmdb

set TOOLS=G:\file\code\dnn\caffe-ssd-win-diy\Build\x64\Release

set root_dir=G:\file\code\dnn\caffe-ssd-win-diy
cd %root_dir%


set data_root_dir=%root_dir%\data\VOCdevkit\
set mapfile=%data_root_dir%\labelmap_voc.prototxt
set anno_type=detection
set db=lmdb
set min_dim=0
set max_dim=0
set width=0
set height=0

set "extra_cmd=--encode_type=jpg --encoded=True"



for %%s in (trainval test) do (
echo Creating %%s lmdb...

%TOOLS%\convert_annoset ^
	--anno_type=%anno_type% ^
	--label_map_file=%mapfile% ^
	--min_dim=%min_dim% ^
	--max_dim=%max_dim% ^
	--resize_width=%width% ^
	--resize_height=%height% ^
	--check_label %extra_cmd% ^
	%data_root_dir% ^
	%data_root_dir%\%%s.txt ^
	%data_root_dir%\%%s_%db%
)

pause