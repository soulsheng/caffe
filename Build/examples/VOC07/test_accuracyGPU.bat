G:
cd ../../x64/Release
caffe test -model ../../models/VOC07/test.prototxt -weights ../../models/VOC07/VGG_VOC07_SSD_300x300_iter_120000.caffemodel -gpu 0 -iterations 100
pause