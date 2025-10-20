-- Project-local settings: use marker folds and real tabs (2 columns)

-- foldmethod is window-local
vim.wo.foldmethod = "marker"

-- Use tabs (no expand), and make a tab display / indent as 2 columns
vim.opt_local.tabstop = 2 -- a literal tab character = 2 columns
vim.opt_local.shiftwidth = 2 -- indentation amount for >>, <<, autoindent, etc.
vim.opt_local.softtabstop = 2 -- insert-mode tab behavior (helps make Tab behave like shiftwidth)
vim.opt_local.expandtab = false -- do NOT convert tabs to spaces; use real tab characters
