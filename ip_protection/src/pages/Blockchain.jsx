import { useEffect, useState } from "react";
import { motion } from "framer-motion";
import { Blocks, Hash, Clock, User, ArrowDown } from "lucide-react";

const pageVariants = {
  initial: { opacity: 0, y: 20 },
  animate: { opacity: 1, y: 0, transition: { duration: 0.4 } },
  exit: { opacity: 0, y: -20, transition: { duration: 0.3 } }
};

export default function Blockchain() {
  const [chain, setChain] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetch("http://localhost:18080/chain")
      .then(r => r.json())
      .then(data => {
        setChain(data);
        setLoading(false);
      })
      .catch(() => setLoading(false));
  }, []);

  return (
    <motion.div variants={pageVariants} initial="initial" animate="animate" exit="exit" className="max-w-4xl mx-auto mt-6 pb-12">
      <div className="flex items-center gap-4 mb-10">
        <div className="p-3 bg-blue-500/20 rounded-xl">
          <Blocks className="w-8 h-8 text-blue-400" />
        </div>
        <div>
          <h2 className="text-3xl font-bold text-white">Blockchain Explorer</h2>
          <p className="text-slate-400">Immutable ledger of registered IP</p>
        </div>
      </div>

      {loading ? (
        <div className="flex justify-center py-20">
          <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-indigo-500"></div>
        </div>
      ) : chain.length === 0 ? (
        <div className="text-center py-20 glass-panel">
          <p className="text-slate-400">No blocks found. The chain is empty.</p>
        </div>
      ) : (
        <div className="space-y-6 relative">
          {/* Connecting line */}
          <div className="absolute left-[28px] top-8 bottom-8 w-0.5 bg-slate-800 z-0"></div>
          
          {chain.map((b, i) => (
            <motion.div
              key={i}
              initial={{ opacity: 0, x: -20 }}
              animate={{ opacity: 1, x: 0 }}
              transition={{ delay: i * 0.1 }}
              className="relative z-10 flex gap-6"
            >
              <div className="flex-shrink-0 flex flex-col items-center">
                <div className="w-14 h-14 rounded-full bg-slate-900 border-2 border-indigo-500/50 flex items-center justify-center shadow-[0_0_15px_rgba(99,102,241,0.2)]">
                  <span className="font-bold text-indigo-400">#{b.index}</span>
                </div>
                {i < chain.length - 1 && (
                  <ArrowDown className="w-5 h-5 text-slate-700 mt-6" />
                )}
              </div>
              
              <div className="glass-panel p-6 flex-1 hover:border-indigo-500/30 transition-colors group">
                <div className="flex justify-between items-start mb-4">
                  <div className="flex items-center gap-2 text-slate-300">
                    <User className="w-4 h-4 text-purple-400" />
                    <span className="font-medium">{b.creator}</span>
                  </div>
                  <div className="flex items-center gap-1.5 text-xs text-slate-500 bg-slate-900/50 px-3 py-1.5 rounded-full">
                    <Clock className="w-3.5 h-3.5" />
                    {new Date(b.timestamp * 1000).toLocaleString()}
                  </div>
                </div>

                <div className="space-y-3">
                  <div className="bg-slate-900/50 p-3 rounded-lg border border-white/5">
                    <div className="flex items-center gap-2 mb-1 text-xs text-slate-400 font-medium">
                      <Hash className="w-3.5 h-3.5 text-indigo-400" />
                      Block Hash
                    </div>
                    <p className="font-mono text-sm text-slate-300 break-all">{b.blockHash || "0"}</p>
                  </div>
                  
                  <div className="grid grid-cols-1 md:grid-cols-2 gap-3">
                    <div className="bg-slate-900/50 p-3 rounded-lg border border-white/5">
                      <div className="text-xs text-slate-400 font-medium mb-1">IP Hash</div>
                      <p className="font-mono text-sm text-slate-300 break-all">{b.ipHash}</p>
                    </div>
                    <div className="bg-slate-900/50 p-3 rounded-lg border border-white/5">
                      <div className="text-xs text-slate-400 font-medium mb-1">Previous Hash</div>
                      <p className="font-mono text-sm text-slate-300 break-all">{b.prevHash}</p>
                    </div>
                  </div>
                </div>
              </div>
            </motion.div>
          ))}
        </div>
      )}
    </motion.div>
  );
}
