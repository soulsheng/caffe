G:
cd ../../x64/Release
caffe test -model ../../models/re5/train_val.prototxt -weights ../../models/re5/re5_train_iter_200.caffemodel -gpu 0 -iterations 2
pause