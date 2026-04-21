import { useEffect, useState } from "react";
import { motion } from "framer-motion";
import { Activity, Blocks, RefreshCw } from "lucide-react";
import { apiGet, apiPost } from "../services/api";

const pageVariants = {
  initial: { opacity: 0, y: 20 },
  animate: { opacity: 1, y: 0, transition: { duration: 0.4 } },
  exit: { opacity: 0, y: -20, transition: { duration: 0.3 } }
};

export default function Dashboard() {
  const [status, setStatus] = useState("loading");
  const [chain, setChain] = useState([]);
  const [message, setMessage] = useState("");
  const [resetting, setResetting] = useState(false);

  async function loadDashboard() {
    setStatus("loading");
    try {
      const [health, chainResponse] = await Promise.all([
        apiGet("/health"),
        apiGet("/chain"),
      ]);
      setStatus(health.ok && health.data.status === "ok" ? "online" : "offline");
      setChain(Array.isArray(chainResponse.data) ? chainResponse.data : []);
    } catch {
      setStatus("offline");
      setChain([]);
    }
  }

  async function handleReset() {
    const confirmed = window.confirm(
      "Reset the blockchain to the genesis block and clear registered users?"
    );
    if (!confirmed) return;

    setResetting(true);
    setMessage("");

    try {
      const { ok, data } = await apiPost("/reset", {});
      if (ok && data.success) {
        setMessage("Blockchain reset on the server. Browser keys were kept.");
        await loadDashboard();
      } else {
        setMessage(data.message || "Reset failed.");
      }
    } catch {
      setMessage("Reset request failed. Check whether the server is running.");
    } finally {
      setResetting(false);
    }
  }

  useEffect(() => {
    loadDashboard();
  }, []);

  return (
    <motion.div variants={pageVariants} initial="initial" animate="animate" exit="exit" className="max-w-4xl mx-auto mt-4">
      <div className="flex flex-col md:flex-row items-center justify-between mb-10 gap-4">
        <div>
          <h1 className="text-4xl font-extrabold bg-gradient-to-r from-white to-slate-400 bg-clip-text text-transparent text-center md:text-left">
            System Dashboard
          </h1>
          <p className="text-slate-400 mt-2 text-center md:text-left">Monitor your ZKP-backed blockchain network</p>
        </div>
        <button
          onClick={handleReset}
          disabled={resetting}
          className="flex items-center gap-2 bg-red-500/20 text-red-400 hover:bg-red-500/30 border border-red-500/30 px-5 py-2.5 rounded-xl font-medium transition-all"
        >
          <RefreshCw className={`w-4 h-4 ${resetting ? "animate-spin" : ""}`} />
          {resetting ? "Resetting..." : "Reset Blockchain"}
        </button>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 gap-6 mb-8">
        <motion.div whileHover={{ y: -4 }} className="glass-panel p-6 flex items-center gap-6">
          <div className={`p-4 rounded-full ${status === 'online' ? 'bg-green-500/20 text-green-400' : status === 'offline' ? 'bg-red-500/20 text-red-400' : 'bg-yellow-500/20 text-yellow-400'}`}>
            <Activity className="w-8 h-8" />
          </div>
          <div>
            <p className="text-slate-400 font-medium">Server Status</p>
            <h2 className="text-2xl font-bold capitalize mt-1 text-white">
              {status}
            </h2>
          </div>
        </motion.div>

        <motion.div whileHover={{ y: -4 }} className="glass-panel p-6 flex items-center gap-6">
          <div className="p-4 rounded-full bg-indigo-500/20 text-indigo-400">
            <Blocks className="w-8 h-8" />
          </div>
          <div>
            <p className="text-slate-400 font-medium">Total Blocks</p>
            <h2 className="text-2xl font-bold mt-1 text-white">{chain.length}</h2>
          </div>
        </motion.div>
      </div>

      {message && (
        <motion.div initial={{ opacity: 0, scale: 0.95 }} animate={{ opacity: 1, scale: 1 }} className="p-4 bg-indigo-500/10 border border-indigo-500/20 text-indigo-300 rounded-xl">
          {message}
        </motion.div>
      )}
    </motion.div>
  );
}
