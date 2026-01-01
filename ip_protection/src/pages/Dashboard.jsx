import { useEffect, useState } from "react";

export default function Dashboard() {
  const [status, setStatus] = useState("loading");
  const [chain, setChain] = useState([]);

  useEffect(() => {
    // health check fetch
    fetch("http://localhost:18080/health")
      .then(res => {
        if (!res.ok) throw new Error("Health check failed");
        return res.json();
      })
      .then(data => {
        if (data.status === "ok") {
          setStatus("online");
        } else {
          setStatus("offline");
        }
      })
      .catch(() => {
        // unreachable or error
        setStatus("offline");
      });

    // chain fetch
    fetch("http://localhost:18080/chain")
      .then(res => res.json())
      .then(data => setChain(data))
      .catch(() => setChain([]));
  }, []);

  return (
    <div className="max-w-3xl mx-auto mt-10 bg-white p-6 rounded shadow">
      <h1 className="text-3xl font-bold mb-6">System Dashboard</h1>

      <p className="text-lg mb-2">
        Server Status:
        {status === "loading" && (
          <span className="ml-2 text-yellow-500 font-semibold">⏳ Checking</span>
        )}
        {status === "online" && (
          <span className="ml-2 text-green-600 font-semibold">● Online</span>
        )}
        {status === "offline" && (
          <span className="ml-2 text-red-600 font-semibold">● Offline</span>
        )}
      </p>

      <p className="text-lg mb-2">
        Total Blocks: <strong>{chain.length}</strong>
      </p>
    </div>
  );
}
