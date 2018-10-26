G:
cd G:/ImageNet/bin/
convert_imageset --resize_height=256 --resize_width=256 --shuffle ../ILSVRC2012_img_val/ ./lable/val.txt ../ilsvrc12_val_lmdb
pause