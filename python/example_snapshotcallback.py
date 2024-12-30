from xcsf import XCS, SnapshotCallback
import numpy as np
import os


ntrain = 500
X = np.random.random((ntrain, 2))
y = np.random.random(ntrain)
if y.ndim == 1:
    y = y.reshape(-1, 1)

X_DIM: int = np.shape(X)[1]
Y_DIM: int = np.shape(y)[1]

model = XCS(
    x_dim=X_DIM,
    y_dim=Y_DIM,
    max_trials=10000,
    perf_trials=100,
    pop_size=200,
)
callback = SnapshotCallback(
    dirpath="snapshots/",
    save_freq=100,
    verbose=True,
)
model.fit(X, y, callbacks=[callback])

# Callbacks are executed after each "epoch", where an epoch is equal to
# perf_trials number of learning trials. The total number of possible epochs
# will therefore be equal to ceil(max_trials/perf_trials) as defined in the
# constructor.


models = []
dpath = "checkpoints/"
for fname in os.listdir(dpath):
    fpath = dpath + "/" + fname
    m = XCS()
    m.load(fpath)
    models.append(m)

times = [m.time() for m in models]
print(sorted(times))
