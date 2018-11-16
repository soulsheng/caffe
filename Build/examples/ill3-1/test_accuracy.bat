G:
cd ../../x64/Release
caffe test -model ../../models/ill3-1/train_val.prototxt -weights ../../models/ill3-1/train_best.caffemodel -gpu 0 -iterations 10
pause