F:
cd F:/ImageNet/bin/
caffe test -model models/bvlc_reference_caffenet/train_val.prototxt -weights models/bvlc_reference_caffenet/caffenet_train_iter_50000.caffemodel -gpu 0 -iterations 1000
pause