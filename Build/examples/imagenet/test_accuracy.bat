G:
cd G:/ImageNet/bin/
caffe test -model models/bvlc_reference_caffenet/train_val.prototxt -weights models/bvlc_reference_caffenet/caffenet_train_iter_940000.caffemodel -gpu 0 -iterations 1000
pause