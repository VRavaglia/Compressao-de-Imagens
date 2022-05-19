import lbg
import numpy as np
import matplotlib.pyplot as plt

testdata = [(48,49),
(50,51),
(52,53),
(54,55),
(97,98),
(99,100),
(101,102),
(103,104)]

cb = []

cb_size = 2
# print('generating codebook for size', cb_size)
cb, cb_abs_w, cb_rel_w = lbg.generate_codebook(testdata, cb_size)
# print('output:')
# for i, c in enumerate(cb):
#     print('> %s, abs_weight=%d, rel_weight=%f' % (c, cb_abs_w[i], cb_rel_w[i]))

testdata = np.array(testdata)
cb = np.array(cb)
plt.scatter(testdata[:, 0], testdata[:, 1])
plt.scatter(cb[:, 0], cb[:, 1])

plt.show()