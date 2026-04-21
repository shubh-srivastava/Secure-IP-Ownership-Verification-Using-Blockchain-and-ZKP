import { Link, useLocation } from "react-router-dom";
import { motion } from "framer-motion";
import { Shield, LayoutDashboard, UserPlus, FilePlus, Fingerprint, Link as LinkIcon } from "lucide-react";

export default function Navbar() {
  const location = useLocation();

  const navItems = [
    { path: "/", label: "Dashboard", icon: LayoutDashboard },
    { path: "/register", label: "Register User", icon: UserPlus },
    { path: "/create", label: "Register IP", icon: FilePlus },
    { path: "/verify", label: "Verify IP", icon: Fingerprint },
    { path: "/chain", label: "Blockchain", icon: LinkIcon },
  ];

  return (
    <nav className="sticky top-0 z-50 pt-4 px-4 sm:px-6 mb-8 w-full max-w-6xl mx-auto">
      <div className="glass-panel flex flex-col md:flex-row items-center justify-between px-6 py-4 gap-4">
        <div className="flex items-center gap-3 text-indigo-400">
          <Shield className="w-8 h-8" />
          <span className="text-xl font-bold tracking-tight bg-gradient-to-r from-indigo-400 to-purple-400 bg-clip-text text-transparent">
            zkIP Ledger
          </span>
        </div>
        
        <div className="flex flex-wrap justify-center gap-2">
          {navItems.map((item) => {
            const isActive = location.pathname === item.path;
            const Icon = item.icon;
            return (
              <Link
                key={item.path}
                to={item.path}
                className={`relative px-4 py-2 rounded-lg text-sm font-medium transition-colors ${
                  isActive ? "text-white" : "text-slate-400 hover:text-slate-200"
                }`}
              >
                <span className="flex items-center gap-2 relative z-10">
                  <Icon className="w-4 h-4" />
                  <span className="hidden sm:inline">{item.label}</span>
                </span>
                {isActive && (
                  <motion.div
                    layoutId="navbar-indicator"
                    className="absolute inset-0 bg-white/10 rounded-lg"
                    initial={false}
                    transition={{ type: "spring", stiffness: 400, damping: 30 }}
                  />
                )}
              </Link>
            );
          })}
        </div>
      </div>
    </nav>
  );
}
