from uwimg import *
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "dogthumb")

# test highpass filter
im = load_image("data/dog.jpg");
f = make_highpass_filter();
blur = convolve_image(im, f, 0);
save_image(blur, "dog-highpass")

im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-gauss2")

im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
lfreq = convolve_image(im, f, 1)
hfreq = im - lfreq
reconstruct = lfreq + hfreq
save_image(lfreq, "low-frequency")
save_image(hfreq, "high-frequency")
save_image(reconstruct, "reconstruct")

# test hybrid
melisa = load_image("data/melisa.png")
aria = load_image("data/aria.png")
f = make_gaussian_filter(2)
lfreq_m = convolve_image(melisa, f, 1)
lfreq_a = convolve_image(aria, f, 1)
hfreq_a = sub_image(aria , lfreq_a)
reconstruct = add_image(lfreq_m , hfreq_a)
clamp_image(reconstruct)
save_image(reconstruct, "hybrid")

im = load_image("data/dog.jpg")
res = sobel_image(im)
mag = res[0]
normalize_image(mag)
save_image(mag, "magnitude")

# EXTRA CREDIT
im = load_image("figs/salt_petter_building.jpg")
med = apply_median_filter(im, 3)
save_image(med, "building-median")
