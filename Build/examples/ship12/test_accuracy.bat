G:
cd G:/ImageNet/bin/
caffe test -model models/ill6/train_val.prototxt -weights models/ill6/ill6_train_iter_20000.caffemodel -gpu 0 -iterations 32
pause